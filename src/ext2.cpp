#include "ext2.h"

MBR* mbr;
Superblock* superblock;
BGD* bgd;

uint32_t working_directory_inode;
uint32_t working_directory;
char* path;

uint32_t block_groups;
uint32_t partition_start;
uint32_t block_size;
uint32_t fragment_size;
uint32_t block_sectors;
uint32_t block_bitmap_size;
uint32_t inode_bitmap_size;

void ext2_init()
{
    mbr = (MBR*)malloc(sizeof(MBR));

    disk_read(mbr, 0, sizeof(MBR));

    partition_start = mbr->partitions[0].start_lba;

    superblock = (Superblock*)malloc(1024);

    disk_read(superblock, partition_start + 2, 1024);

    block_size = 1024 << superblock->block_size_log;
    block_sectors = block_size / 512;
    fragment_size = 1024 << superblock->fragment_size_log;

    block_groups = round(superblock->blocks / (double)superblock->blocks_in_group);

    block_bitmap_size = superblock->blocks_in_group / 8;
    inode_bitmap_size = superblock->inodes_in_group / 8;

    bgd = (BGD*)malloc(block_groups * sizeof(BGD));

    uint32_t bgd_start = partition_start + block_sectors;
    disk_read(bgd, bgd_start, block_groups * sizeof(BGD));

    Inode* root = (Inode*)malloc(sizeof(Inode));
    ext2_read_inode(2, root);

    working_directory_inode = 2;
    working_directory = partition_start + root->block_ptr[0] * block_sectors;

    free(root);

    path = (char*)calloc(256);
    strcat(path, "Root");
}

void ext2_read_inode(uint32_t inode_number, Inode* inode)
{
    uint32_t block_group = (inode_number - 1) / superblock->inodes_in_group;
    uint32_t index = (inode_number - 1) % superblock->inodes_in_group;

    uint32_t sector_offset = (index * superblock->inode_size) / 512;
    uint32_t inode_offset = (index * superblock->inode_size) % 512;

    uint8_t* buffer = (uint8_t*)malloc(512);

    disk_read(buffer, partition_start + bgd[block_group].inode_table * block_sectors + sector_offset, 512);

    memcpy(inode, buffer + inode_offset, sizeof(Inode));

    free(buffer);
}

void ext2_write_inode(uint32_t inode_number, Inode* inode)
{
    uint32_t block_group = (inode_number - 1) / superblock->inodes_in_group;
    uint32_t index = (inode_number - 1) % superblock->inodes_in_group;

    uint32_t sector_offset = (index * superblock->inode_size) / 512;
    uint32_t inode_offset = (index * superblock->inode_size) % 512;

    uint8_t* buffer = (uint8_t*)malloc(512);

    disk_read(buffer, partition_start + bgd[block_group].inode_table * block_sectors + sector_offset, 512);

    memcpy(buffer + inode_offset, inode, sizeof(Inode));

    disk_write(buffer, partition_start + bgd[block_group].inode_table * block_sectors + sector_offset, 512);

    free(buffer);
}

bool ext2_read_dirent(char* name, Dirent* dirent)
{
    uint8_t* buffer = (uint8_t*)malloc(4096);

    disk_read(buffer, working_directory, 4096);

    int next = 0;

    while (1)
    {
        Dirent* dir = (Dirent*)(buffer + next);

        if (dir->inode == 0)
        {
            free(buffer);
            return false;
        }

        if (match(dir->name, name))
        {
            memcpy(dirent, dir, sizeof(Dirent));
            free(buffer);
            return true;
        }

        next += dir->entry_size;

        if (next + (4 + 2 + 1 + 1 + 1 + 1) >= 4096)
        {
            free(buffer);
            return false;
        }
    }

    free(buffer);
    return false;
}

void ext2_write_dirent(Dirent* dummy_dirent)
{
    uint8_t* buffer = (uint8_t*)malloc(4096);

    disk_read(buffer, working_directory, 4096);

    int next = 0;

    while (1)
    {
        Dirent* dirent = (Dirent*)(buffer + next);

        if (dirent->inode == 0)
        {
            free(buffer);
            return;
        }

        int new_entry_size = 4 + 2 + 1 + 1 + dummy_dirent->name_length + 1;
        int actual_entry_size = 4 + 2 + 1 + 1 + dirent->name_length + 1;

        int space = dirent->entry_size - actual_entry_size;

        if (space > new_entry_size)
        {
            int old_entry_size = dirent->entry_size;

            dirent->entry_size = actual_entry_size;

            next += actual_entry_size;

            dirent = (Dirent*)(buffer + next);
            memcpy(dirent, dummy_dirent, new_entry_size);

            dirent->entry_size = old_entry_size - actual_entry_size;

            disk_write(buffer, working_directory, 4096);

            free(buffer);
            return;
        }

        next += dirent->entry_size;

        if (next + (4 + 2 + 1 + 1 + 1 + 1) >= 4096)
        {
            free(buffer);
            return;
        }
    }

    free(buffer);
    return;
}

uint32_t ext2_allocate_block()
{
    uint8_t* buffer = (uint8_t*)malloc(block_bitmap_size);

    disk_read(buffer, partition_start + bgd[0].block_bitmap * block_sectors, block_bitmap_size);

    Bitmap bitmap = { block_bitmap_size, buffer };

    for (uint32_t i = 16; i < block_bitmap_size * 8; i++)
    {
        if (bitmap[i] == 0)
        {
            bitmap.set(i, true);

            disk_write(buffer, partition_start + bgd[0].block_bitmap * block_sectors, block_bitmap_size);

            free(buffer);

            return i;
        }
    }

    free(buffer);

    return 0;
}

void ext2_unallocate_block(uint32_t index)
{
    uint8_t* buffer = (uint8_t*)malloc(block_bitmap_size);

    disk_read(buffer, partition_start + bgd[0].block_bitmap * block_sectors, block_bitmap_size);

    Bitmap bitmap = { block_bitmap_size, buffer };

    bitmap.set(index, false);

    disk_write(buffer, partition_start + bgd[0].block_bitmap * block_sectors, block_bitmap_size);

    free(buffer);
}

uint32_t ext2_allocate_inode()
{
    uint8_t* buffer = (uint8_t*)malloc(inode_bitmap_size);

    disk_read(buffer, partition_start + bgd[0].inode_bitmap * block_sectors, inode_bitmap_size);

    Bitmap bitmap = { inode_bitmap_size, buffer };

    for (uint32_t i = superblock->first_nonreserved_inode; i < inode_bitmap_size * 8; i++)
    {
        if (bitmap[i] == 0)
        {
            bitmap.set(i, true);

            disk_write(buffer, partition_start + bgd[0].inode_bitmap * block_sectors, inode_bitmap_size);

            free(buffer);

            return i + 1;
        }
    }

    free(buffer);

    return 0;
}

void ext2_unallocate_inode(uint32_t index)
{
    uint8_t* buffer = (uint8_t*)malloc(inode_bitmap_size);

    disk_read(buffer, partition_start + bgd[0].inode_bitmap * block_sectors, inode_bitmap_size);

    Bitmap bitmap = { inode_bitmap_size, buffer };

    bitmap.set(index - 1, false);

    disk_write(buffer, partition_start + bgd[0].inode_bitmap * block_sectors, inode_bitmap_size);

    free(buffer);
}

void ext2_chdir(uint32_t inode_number)
{
    Inode* inode = (Inode*)malloc(sizeof(Inode));

    ext2_read_inode(inode_number, inode);

    working_directory_inode = inode_number;
    working_directory = partition_start + inode->block_ptr[0] * block_sectors;

    free(inode);
}

void ext2_load(Inode* inode, uint8_t* buffer, uint64_t size)
{
    uint64_t file_size = ((uint64_t)inode->size_high << 32) + inode->size_low;

    if (size > file_size)
        size = file_size;

    int full_blocks = size / block_size;
    int last_block = size % block_size;

    int i = 0;

    for (;i < full_blocks;i++)
        disk_read(buffer + i * block_size, partition_start + inode->block_ptr[i] * block_sectors, block_size);

    if (last_block != 0)
        disk_read(buffer + i * block_size, partition_start + inode->block_ptr[i] * block_sectors, last_block);
}

uint32_t ext2_create(char* name, uint8_t type)
{
    uint32_t inode_number = ext2_allocate_inode();

    Inode* inode = (Inode*)calloc(sizeof(Inode));
    inode->type = 0b0000000111111111;
    inode->hardlinks = 1;

    ext2_write_inode(inode_number, inode);

    free(inode);

    int name_length = strlen(name);

    Dirent* dirent = (Dirent*)calloc(sizeof(Dirent));
    dirent->inode = inode_number;
    dirent->type = type;
    dirent->name_length = name_length;
    strncpy(dirent->name, name, name_length);
    dirent->entry_size = 4 + 2 + 1 + 1 + name_length + 1;

    ext2_write_dirent(dirent);

    free(dirent);

    return inode_number;
}

void ext2_remove(Dirent* dirent)
{
    Inode* inode = (Inode*)malloc(sizeof(Inode));

    ext2_read_inode(dirent->inode, inode);

    if (inode->hardlinks == 1)
    {
        for (int i = 0;i < 12;i++)
        {
            if (inode->block_ptr[i] == 0)
                break;

            ext2_unallocate_block(inode->block_ptr[i]);
        }

        ext2_unallocate_inode(dirent->inode);
    }
    else
    {
        inode->hardlinks--;
        ext2_write_inode(dirent->inode, inode);
    }

    free(inode);

    uint8_t* buffer = (uint8_t*)malloc(4096);

    disk_read(buffer, working_directory, 4096);

    Dirent* back_dir = 0;

    int next = 0;
    while (1)
    {
        Dirent* dir = (Dirent*)(buffer + next);

        if (dir->inode == 0)
            break;

        if (match(dirent->name, dir->name))
        {
            if (back_dir != 0)
            {
                back_dir->entry_size += dir->entry_size;
                int actual_entry_size = 4 + 2 + 1 + 1 + dir->name_length + 1;
                memset(dir, 0, actual_entry_size);

                disk_write(buffer, working_directory, 4096);
                break;
            }
        }

        next += dir->entry_size;
        back_dir = dir;

        if (next + (4 + 2 + 1 + 1 + 1 + 1) >= 4096)
            break;
    }

    free(buffer);
}

void ext2_mkdir(char* name)
{
    int inode_number = ext2_create(name, 2);

    uint8_t* buffer = (uint8_t*)calloc(block_size);

    Dirent* dot = (Dirent*)buffer;
    dot->inode = inode_number;
    dot->type = 2;
    dot->name_length = 1;
    dot->name[0] = '.';
    dot->entry_size = 4 + 2 + 1 + 1 + 1 + 1;

    Dirent* dotdot = (Dirent*)(buffer + dot->entry_size);
    dotdot->inode = working_directory_inode;
    dotdot->type = 2;
    dotdot->name_length = 2;
    dotdot->name[0] = '.';
    dotdot->name[1] = '.';
    dotdot->entry_size = block_size - dot->entry_size;

    Inode* inode = (Inode*)malloc(sizeof(Inode));

    ext2_read_inode(inode_number, inode);

    ext2_write(inode_number, inode, buffer, block_size);

    free(inode);

    free(buffer);
}

void ext2_rmdir(Dirent* dirent)
{
    int save_inode = working_directory_inode;

    ext2_chdir(dirent->inode);

    uint8_t* buffer = (uint8_t*)malloc(block_size);

    disk_read(buffer, working_directory, block_size);

    int next = 0;
    while (1)
    {
        Dirent* dir = (Dirent*)(buffer + next);

        if (dir->inode == 0)
            break;

        if (dir->type == 2)
        {
            if (!match(dir->name, ".") && !match(dir->name, ".."))
                ext2_rmdir(dir);
        }
        else
            ext2_remove(dir);

        next += dir->entry_size;

        if (next + (4 + 2 + 1 + 1 + 1 + 1) >= block_size)
            break;
    }

    free(buffer);

    ext2_chdir(save_inode);

    ext2_remove(dirent);
}

void ext2_write(uint32_t inode_number, Inode* inode, uint8_t* buffer, uint64_t size)
{
    uint64_t file_size = ((uint64_t)inode->size_high << 32) + inode->size_low;

    int blocks = 0;

    for (int i = 0; i < 12; i++)
    {
        if (inode->block_ptr[i] == 0)
            break;

        blocks++;
    }

    int new_blocks = (size / block_size) + ((size % block_size) ? 1 : 0);

    if (new_blocks > blocks)
    {
        for (int i = blocks; i < new_blocks; i++)
            inode->block_ptr[i] = ext2_allocate_block();

        blocks = new_blocks;
    }
    else if (new_blocks < blocks)
    {
        for (int i = blocks - 1; i >= new_blocks; i--)
        {
            ext2_unallocate_block(inode->block_ptr[i]);
            inode->block_ptr[i] = 0;
        }

        blocks = new_blocks;
    }

    inode->size_low = size & 0xFFFFFFFF;
    inode->size_high = (size >> 32) & 0xFFFFFFFF;

    ext2_write_inode(inode_number, inode);

    bool full_write = (size % block_size == 0);

    for (int i = 0;i < blocks;i++)
    {
        if ((i != (blocks - 1)) || full_write)
        {
            disk_write(buffer + i * block_size, partition_start + inode->block_ptr[i] * block_sectors, block_size);
        }
        else
        {
            uint8_t* clean_buffer = (uint8_t*)calloc(block_size);
            memcpy(clean_buffer, buffer + i * block_size, size % block_size);

            disk_write(clean_buffer, partition_start + inode->block_ptr[i] * block_sectors, block_size);

            free(clean_buffer);
        }
    }
}

void ext2_list(uint32_t start) //tmp
{
    uint8_t* buffer = (uint8_t*)malloc(4096);

    disk_read(buffer, start, 4096);

    int next = 0;
    while (1)
    {
        Dirent* dirent = (Dirent*)(buffer + next);

        if (dirent->inode == 0)
            break;

        if (dirent->type == 2)
            colorf = BLUE;

        printf("%s   ", dirent->name);

        if (dirent->type == 2)
            colorf = FRONT;

        next += dirent->entry_size;

        if (next + (4 + 2 + 1 + 1 + 1 + 1) >= 4096)
            break;
    }

    printf("\n");

    free(buffer);
}

void print_path()
{
    char* ptr = path;

    while (*ptr != 0)
    {
        colorf = (*ptr == '/') ? RED : BLUE;
        printf("%c", *ptr);

        ptr++;
    }

    colorf = FRONT;
}

void print_mbr(MBR* mbr)
{
    printf("\n-------- Master Boot Record --------\n\n");

    for (int i = 0;i < 4;i++)
    {
        Partition part = mbr->partitions[i];
        printf("Partition %d:    status = %x    type = %x    start_LBA = %d    length = %d\n",
            i, part.status, part.type, part.start_lba, part.length);
    }
}

void print_superblock(Superblock* sb)
{
    printf("\n-------- Ext2 Superblock --------\n\n");

    printf("Total inodes: %d\nTotal blocks: %d\nUnallocated inodes: %d\nUnallocated blocks: %d\nBlock number of superblock : %d\nBlock size : %d\nFragment size : %d\nBlocks in a block group : %d\nFragments in a block group : %d\nInodes in a block group : %d\nFilesystem state : %d\nVersion: m = %d M = %d\n",
        sb->inodes, sb->blocks, sb->unalloc_inodes, sb->unalloc_blocks, sb->block_of_superblock, 1024 << sb->block_size_log,
        1024 << sb->fragment_size_log, sb->blocks_in_group, sb->fragments_in_group, sb->inodes_in_group, sb->state, sb->version_minor, sb->version_major);

    if (sb->version_major >= 1)
    {
        printf("First non-reserved inode: %d\nInode size: %d\nFilesystem ID: %d\nVolume name: %s\nLast mount path: %s\nPreallocate blocks for file: %d\nPreallocate blocks for directory: %d\n",
            sb->first_nonreserved_inode, sb->inode_size, sb->filesystem_id, sb->volume_name, sb->last_path, sb->prealloc_blocks_file, sb->prealloc_blocks_dir);
    }
}

void print_dirent(Dirent* dirent)
{
    printf("\nDirectory Entry: \n");
    printf("\tType: %d\n", dirent->type);
    printf("\tInode: %d\n", dirent->inode);
    printf("\tEntry size: %d\n", dirent->entry_size);
    printf("\tName length: %d\n", dirent->name_length);
    printf("\tName: %s\n", dirent->name);
}

void print_inode(Inode* inode)
{
    char perm[] = "----------";

    if (inode->type & 1)
        perm[9] = 'x';

    if ((inode->type >> 1) & 1)
        perm[8] = 'w';

    if ((inode->type >> 2) & 1)
        perm[7] = 'r';

    if ((inode->type >> 3) & 1)
        perm[6] = 'x';

    if ((inode->type >> 4) & 1)
        perm[5] = 'w';

    if ((inode->type >> 5) & 1)
        perm[4] = 'r';

    if ((inode->type >> 6) & 1)
        perm[3] = 'x';

    if ((inode->type >> 7) & 1)
        perm[2] = 'w';

    if ((inode->type >> 8) & 1)
        perm[1] = 'r';

    switch (inode->type >> 12)
    {
    case 1:
        perm[0] = 'p';
        break;
    case 2:
        perm[0] = 'c';
        break;
    case 4:
        perm[0] = 'd';
        break;
    case 6:
        perm[0] = 'b';
        break;
    case 8:
        perm[0] = '-';
        break;
    case 10:
        perm[0] = 'l';
        break;
    case 12:
        perm[0] = 's';
        break;
    default:
        perm[0] = '?';
        break;
    }

    printf("Type and permissions: %s\n", perm);

    printf("Size: %d bytes\n", ((uint64_t)inode->size_high << 32) + inode->size_low);
    printf("Hardlinks: %d\n", inode->hardlinks);

    printf("Block pointers: { ");

    for (int i = 0;i < 12;i++)
    {
        printf("%d", inode->block_ptr[i]);

        if (i != 11)
            printf(", ");
    }

    printf(" }\n");
}