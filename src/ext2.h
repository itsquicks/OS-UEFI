#pragma once
#include "defs.h"
#include "heap.h"
#include "utils.h"
#include "disk.h"
#include "math.h"

struct Partition
{
    uint8_t status;
    uint8_t start_head;
    uint8_t start_sector;
    uint8_t start_cylinder;
    uint8_t type;
    uint8_t end_head;
    uint8_t end_sector;
    uint8_t end_cylinder;
    uint32_t start_lba;
    uint32_t length;

}__attribute__((packed));

struct MBR
{
    uint8_t bootloader[440];
    uint32_t signature;
    uint16_t unused;
    Partition partitions[4];
    uint16_t magic_number;

}__attribute__((packed));

struct Superblock
{
    uint32_t inodes;
    uint32_t blocks;
    uint32_t reserved_blocks;
    uint32_t unalloc_blocks;
    uint32_t unalloc_inodes;
    uint32_t block_of_superblock;
    uint32_t block_size_log;
    uint32_t fragment_size_log;
    uint32_t blocks_in_group;
    uint32_t fragments_in_group;
    uint32_t inodes_in_group;
    uint32_t last_mount_time;
    uint32_t last_written_time;
    uint16_t con_check;
    uint16_t con_check_max;
    uint16_t signature;
    uint16_t state;
    uint16_t when_error;
    uint16_t version_minor;
    uint32_t last_con_check;
    uint32_t time_between_forced_con_check;
    uint32_t os_id;
    uint32_t version_major;
    uint16_t user_id;
    uint16_t group_id;

    //Extended if version_major >= 1

    uint32_t first_nonreserved_inode;
    uint16_t inode_size;
    uint16_t group_of_superblock;
    uint32_t opt_features;
    uint32_t req_features;
    uint32_t readonly_features;
    char filesystem_id[16];
    char volume_name[16];
    char last_path[64];
    uint32_t compression_algorithms;
    uint8_t prealloc_blocks_file;
    uint8_t prealloc_blocks_dir;
    uint16_t unused;
    char journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t head_orphan;
    char unused2[788];

}__attribute__((packed));

struct BGD
{
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t unalloc_blocks;
    uint16_t unalloc_inodes;
    uint16_t dir_in_group;
    uint8_t unused[14];

}__attribute__((packed));

struct Inode
{
    uint16_t type;
    uint16_t userID;
    uint32_t size_low;
    uint32_t last_access_time;
    uint32_t creation_time;
    uint32_t last_mod_time;
    uint32_t deletion_time;
    uint16_t groupID;
    uint16_t hardlinks;
    uint32_t disk_sectors;
    uint32_t flags;
    uint32_t os_spec1;
    uint32_t block_ptr[12];
    uint32_t singly_block_ptr;
    uint32_t doubly_block_ptr;
    uint32_t triply_block_ptr;
    uint32_t gen;
    uint32_t extended_attribute_block;
    uint32_t size_high;
    uint32_t fragment;
    char os_spec2[12];

}__attribute__((packed));

struct Dirent
{
    uint32_t inode;
    uint16_t entry_size;
    uint8_t name_length;
    uint8_t type;
    char name[256];

}__attribute__((packed));

extern MBR* mbr;
extern Superblock* superblock;

extern uint32_t working_directory_inode;
extern uint32_t working_directory;
extern char* path;

void ext2_init();

void ext2_read_inode(uint32_t inode_number, Inode* inode);
void ext2_write_inode(uint32_t inode_number, Inode* inode);
bool ext2_read_dirent(char* name, Dirent* dirent);
void ext2_write_dirent(Dirent* dummy_dirent);

void ext2_chdir(uint32_t inode_number);
void ext2_load(Inode* inode, uint8_t* buffer, uint64_t size);
uint32_t ext2_create(char* name, uint8_t type = 1);
void ext2_remove(Dirent* dirent);
void ext2_mkdir(char* name);
void ext2_rmdir(Dirent* dirent);
void ext2_write(uint32_t inode_number, Inode* inode, uint8_t* buffer, uint64_t size);
void ext2_list(uint32_t start = working_directory); //tmp

void print_path();
void print_mbr(MBR* mbr);
void print_superblock(Superblock* superblock);
void print_inode(Inode* inode);
void print_dirent(Dirent* dirent);