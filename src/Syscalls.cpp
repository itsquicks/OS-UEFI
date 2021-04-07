#include "Syscalls.h"

bool debug = true;

uint64 rax = 0;
uint64 rbx = 0;
uint64 rcx = 0;
uint64 rdx = 0;
uint64 rdi = 0;

uint64 SyscallsHandler()
{
    asm("movq %%rax, %0" : "=a"(rax));
    asm("movq %%rbx, %0" : "=b"(rbx));
    asm("movq %%rcx, %0" : "=c"(rcx));
    asm("movq %%rdx, %0" : "=d"(rdx));
    asm("movq %%rdi, %0" : "=D"(rdi));

    if (debug)
    {
        colorf = YELLOW;
        PrintString("Syscall: ");
        colorf = FRONT;
    }

    switch (rbx)
    {
    case 1:
        return syscall_open();
    case 2:
        return syscall_close();
    case 3:
        return syscall_remove();
    case 4:
        return syscall_rmdir();
    case 5:
        return syscall_mkdir();
    case 6:
        return syscall_chdir();
    case 7:
        return syscall_pwd();
    }

    return 0;
}

int syscall_open()
{
    if (debug)
        PrintString("syscall_open\n");

    DirectoryEntry* file = DirectoryFromName((char*)rdi);

    if (file != 0)
    {
        if (file->attributes & FLAG_FOLDER)
            return 0;
        else
        {
            void* address = RequestPage();
            LockPage(address);
            memset(address, 0, 4096);

            LoadFile(*file, (uint64)address);

            FILE* ret = (FILE*)RequestPage();
            LockPage(ret);
            memset(ret, 0, 4096);

            ret->flags = 0;
            ret->buffer_base = (uint8*)address;
            ret->buffer_end = (uint8*)((uint64)address + file->size);
            ret->read_ptr = ret->buffer_base;
            ret->write_ptr = ret->buffer_base;
            ret->fileno = 3;
            *(char*)((uint64)ret->buffer_end) = -1;

            return (uint64)ret;
        }
    }
    else return 0;
}

int syscall_close()
{
    if (debug)
        PrintString("syscall_close\n");

    FILE* file = (FILE*)rdi;

    FreePage(file->buffer_base);
    FreePage(file);

    return 0;
}

int syscall_remove()
{
    if (debug)
        PrintString("syscall_remove\n");

    DirectoryEntry* file = DirectoryFromName((char*)rdi);

    if (file == 0)
    {
        return 1;
    }
    else
    {
        if (file->attributes & FLAG_FOLDER)
            return 2;
        else
        {
            DeleteFile(*file);
            return 0;
        }
    }
}

int syscall_rmdir()
{
    if (debug)
        PrintString("syscall_rmdir\n");

    char* dirname = (char*)rdi;

    DirectoryEntry* dir = DirectoryFromName(dirname);

    if (dir == 0)
    {
        return 1;
    }
    else
    {
        if (dir->attributes & FLAG_FOLDER)
        {
            if (dir->name[0] == '.')
                return 3;
            else
            {
                DeleteFile(*dir);
                return 0;
            }
        }
        else return 2;
    }
}

int syscall_mkdir()
{
    if (debug)
        PrintString("syscall_mkdir\n");

    NewFile((char*)rdi, FLAG_FOLDER);
    return 0;
}

int syscall_chdir()
{
    if (debug)
        PrintString("syscall_chdir\n");

    char* dirname = (char*)rdi;

    DirectoryEntry* dir = DirectoryFromName(dirname);

    if ((dir->attributes & FLAG_FOLDER) && dir != 0)
    {
        ChangeDirectory(*dir);

        if (Match(dirname, ".."))
        {
            int i = strlen(path);
            while (*(path + i) != '/')
            {
                *(path + i) = 0;
                i--;
            }
            *(path + i) = 0;
        }
        else if (!Match(dirname, "."))
        {
            strcat(path, "/");
            strcat(path, dirname);
        }

        return 0;
    }
    else return 1;
}

int syscall_pwd()
{
    if (debug)
        PrintString("syscall_cwd\n");

    return (uint64)path;
}