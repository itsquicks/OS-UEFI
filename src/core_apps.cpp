#include "core_apps.h"

void cls()
{
    textmode_set_cursor(0);
    textmode_clear_screen();
}

void info()
{
    colorf = CYAN;
    printf("CPU: ");
    colorf = FRONT;
    printf("%s\n", cpu_name);
    colorf = CYAN;
    printf("Speed: ");
    colorf = FRONT;
    printf("%d MHz\n", cpu_get_speed());
    colorf = CYAN;
    printf("Flags: ");
    colorf = FRONT;
    printf("EAX = 0x%x   EBX = 0x%x   ECX = 0x%x   EDX = 0x%x\n", *(cpu_flags), *(cpu_flags + 1), *(cpu_flags + 2), *(cpu_flags + 3));
    colorf = YELLOW;
    printf("Resolution: ");
    colorf = FRONT;
    printf("%dx%d\n", framebuffer->width, framebuffer->height);
    colorf = GREEN;
    printf("Total RAM: ");
    colorf = FRONT;
    printf("%d KB\n", total_memory / KB);
    colorf = GREEN;
    printf("Reserved RAM: ");
    colorf = FRONT;
    printf("%d KB\n", reserved_memory / KB);
    colorf = GREEN;
    printf("Used RAM: ");
    colorf = FRONT;
    printf("%d KB\n", used_memory / KB);
    colorf = GREEN;
    printf("Free RAM: ");
    colorf = FRONT;
    printf("%d KB\n", free_memory / KB);
}

void load(int argc, char** argv)
{
    if (argc == 4)
    {
        char* name = argv[1];
        uint8_t* buffer = (uint8_t*)stox(argv[2]);
        uint64_t size = stoi(argv[3]);

        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            Inode* inode = (Inode*)malloc(sizeof(Inode));

            ext2_read_inode(dirent->inode, inode);

            ext2_load(inode, buffer, size);

            free(inode);
        }
        else
            printf("There is no file named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("load <file> <address> <size>\n");
    }
}

void create(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];
        ext2_create(name);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("create <name>\n");
    }
}

void write(int argc, char** argv)
{
    if (argc == 4)
    {
        char* name = argv[1];
        uint8_t* buffer = (uint8_t*)stox(argv[2]);
        uint64_t size = stoi(argv[3]);

        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            Inode* inode = (Inode*)malloc(sizeof(Inode));

            ext2_read_inode(dirent->inode, inode);

            ext2_write(dirent->inode, inode, buffer, size);

            free(inode);
        }
        else
            printf("There is no file named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("write <file> <address> <size>\n");
    }
}

void ls()
{
    ext2_list();
}

void cat(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];

        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            if (dirent->type == 1)
            {
                Inode* inode = (Inode*)malloc(sizeof(Inode));

                ext2_read_inode(dirent->inode, inode);

                uint64_t file_size = ((uint64_t)inode->size_high << 32) + inode->size_low;

                if (file_size > 0)
                {
                    uint8_t* buffer = (uint8_t*)malloc(file_size);

                    ext2_load(inode, buffer, file_size);

                    printf("%s", buffer);

                    free(buffer);
                }
                free(inode);
            }
            else
                printf("\"%s\" is not a file.\n", name);
        }
        else
            printf("There is no file named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("cat <file>\n");
    }
}

void cd(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];
        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            if (dirent->type == 2)
            {
                ext2_chdir(dirent->inode);

                if (match(name, ".."))
                {
                    if (!match(path, "Root"))
                    {
                        uint8_t i = strlen(path);
                        while (path[i] != '/')
                        {
                            path[i] = 0;
                            i--;
                        }
                        path[i] = 0;
                    }
                }
                else if (!match(name, "."))
                {
                    strcat(path, "/");
                    strcat(path, name);
                }
            }
            else
                printf("\"%s\" is not a directory.\n", name);
        }
        else
            printf("There is no directory named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("cd <directory>\n");
    }
}

void sb(int argc, char** argv)
{
    if (argc >= 3)
    {
        uint8_t* ptr = (uint8_t*)stox(argv[1]);
        for (uint8_t i = 2; i < argc; i++)
        {
            if (argv[i][0] == '~')
            {
                ptr++;
                continue;
            }

            uint8_t val = (uint8_t)stox(argv[i]);
            *ptr = val;
            ptr++;
        }
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("setbytes <address> ...\n");
    }
}

void memdmp(int argc, char** argv)
{
    if (argc == 3)
    {
        uint8_t* ptr = (uint8_t*)stox(argv[1]);

        for (uint64_t i = 0; i < stoi(argv[2]); i++)
        {
            uint8_t* ascii = ptr;

            colorf = GREEN;
            printf("0x%p     ", ptr);
            colorf = FRONT;

            for (uint8_t n = 0; n < 16; n++)
            {
                if (n == 8)
                    printf(" ");

                printf("%x ", *ptr);
                ptr++;
            }
            printf("     ");

            for (uint8_t n = 0; n < 16; n++)
            {
                if (*ascii > 31)
                {
                    printf("%c", *ascii);
                }
                else
                {
                    colorf = GRAY;
                    printf(".");
                    colorf = FRONT;
                }

                ascii++;
            }
            printf("\n");
        }
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("memdmp <address> <rows>\n");
    }
}

void memcpy_app(int argc, char** argv)
{
    if (argc == 4)
    {
        uint64_t* dest = (uint64_t*)stox(argv[1]);
        uint64_t* src = (uint64_t*)stox(argv[2]);
        uint64_t count = stoi(argv[3]);

        memcpy(dest, src, count);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("memcpy <dest> <src> <count>\n");
    }
}

void memset_app(int argc, char** argv)
{
    if (argc == 4)
    {
        uint64_t* dest = (uint64_t*)stox(argv[1]);
        uint8_t value = stox(argv[2]);
        uint64_t count = stoi(argv[3]);

        memsetbyte(dest, value, count);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("memset <dest> <value> <count>\n");
    }
}

void exec(int argc, char** argv)
{
    if (argc == 2)
    {
        void (*main) (void);
        main = (void (*)(void))stox(argv[1]);

        asm("pushq %rax");
        asm("pushq %rcx");
        asm("pushq %rdx");
        asm("pushq %rbx");
        asm("pushq %rsi");
        asm("pushq %rdi");

        main();

        asm("popq %rdi");
        asm("popq %rsi");
        asm("popq %rbx");
        asm("popq %rdx");
        asm("popq %rcx");
        asm("popq %rax");
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("exec <address>\n");
    }
}

void inode_app(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];
        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            Inode* inode = (Inode*)malloc(sizeof(Inode));

            ext2_read_inode(dirent->inode, inode);

            printf("Inode: %d\n", dirent->inode);

            print_inode(inode);

            free(inode);
        }
        else
            printf("There is no file named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("inode <file>\n");
    }
}

void remove(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];

        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            if (dirent->type == 1)
            {
                ext2_remove(dirent);
            }
            else
                printf("\"%s\" is not a file.\n", name);
        }
        else
            printf("There is no file named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("remove <file>\n");
    }
}

void mkdir(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];
        ext2_mkdir(name);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("mkdir <name>\n");
    }
}

void rmdir(int argc, char** argv)
{
    if (argc == 2)
    {
        char* name = argv[1];

        Dirent* dirent = (Dirent*)malloc(sizeof(Dirent));

        if (ext2_read_dirent(name, dirent))
        {
            if (dirent->type == 2)
            {
                if (!match(dirent->name, ".") && !match(dirent->name, ".."))
                    ext2_rmdir(dirent);
                else
                    printf("Directories \".\" and \"..\" can't be removed.\n");
            }
            else
                printf("\"%s\" is not a directory.\n", name);
        }
        else
            printf("There is no directory named \"%s\".\n", name);

        free(dirent);
    }
    else
    {
        colorf = YELLOW;
        printf("Use: ");
        colorf = FRONT;
        printf("rmdir <directory>\n");
    }
}

void heap()
{
    print_heap();
}