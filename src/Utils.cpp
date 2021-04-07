#include "Utils.h"

void cls()
{
    SetCursorPosition(0);
    ClearScreen();
}

void mem()
{
    colorf = GREEN;
    PrintString("Total RAM: ");
    colorf = FRONT;
    PrintString(itos(totalMemory / KB)); PrintString(" KB\n");
    colorf = GREEN;
    PrintString("Reserved RAM: ");
    colorf = FRONT;
    PrintString(itos(reservedMemory / KB)); PrintString(" KB\n");
    colorf = GREEN;
    PrintString("Used RAM: ");
    colorf = FRONT;
    PrintString(itos(usedMemory / KB)); PrintString(" KB\n");
    colorf = GREEN;
    PrintString("Free RAM: ");
    colorf = FRONT;
    PrintString(itos(freeMemory / KB)); PrintString(" KB\n");
}

void ls()
{
    PrintChar('\n');
    PrintDirectoryContent();
    PrintChar('\n');
}

void load(int argc, char** argv)
{
    if (argc == 3)
    {
        DirectoryEntry* dir = DirectoryFromName(argv[1]);
        LoadFile(*dir, stox(argv[2]));
    }
    else
    {
        colorf = YELLOW;
        PrintString("Syntax: ");
        colorf = FRONT;
        PrintString("load 'filename' 'address'\n");
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
        PrintString("Syntax: ");
        colorf = FRONT;
        PrintString("exec 'address'\n");
    }
}

void setbytes(int argc, char** argv)
{
    if (argc >= 3)
    {
        uint8* ptr = (uint8*)stox(argv[1]);
        for (uint8 i = 2; i < argc; i++)
        {
            if (argv[i][0] == '~')
            {
                ptr++;
                continue;
            }

            uint8 val = (uint8)stox(argv[i]);
            *ptr = val;
            ptr++;
        }
    }
    else
    {
        colorf = YELLOW;
        PrintString("Syntax: ");
        colorf = FRONT;
        PrintString("setbytes 'address' '00' ...\n");
    }
}

void memdmp(int argc, char** argv)
{
    if (argc == 3)
    {
        uint8* ptr = (uint8*)stox(argv[1]);

        for (uint8 i = 0; i < stoi(argv[2]); i++)
        {
            uint8* ascii = ptr;

            colorf = GREEN;
            PrintString("0x");  PrintString(xtos((uint64)ptr));
            colorf = FRONT;
            PrintString("     ");

            for (uint8 n = 0; n < 16; n++)
            {
                PrintString(xtos(*ptr));
                PrintChar(' ');
                ptr++;
            }
            PrintString("     ");

            for (uint8 n = 0; n < 16; n++)
            {
                if (*ascii > 31)
                    PrintChar(*ascii);
                else
                {
                    colorf = GRAY;
                    PrintChar('.');
                    colorf = FRONT;
                }

                ascii++;
            }
            PrintChar('\n');
        }
    }
    else
    {
        colorf = YELLOW;
        PrintString("Syntax: ");
        colorf = FRONT;
        PrintString("memdmp 'address' 'rows'\n");
    }
}

void create(int argc, char** argv)
{
    if (argc == 2)
    {
        char* filename = argv[1];
        NewFile(filename, 0);
    }
}

void save(int argc, char** argv)
{
    if (argc == 4)
    {
        char* filename = argv[1];

        DirectoryEntry* dir = DirectoryFromName(filename);

        SaveFile(*dir, stox(argv[2]), stoi(argv[3]));
    }
}

void del(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 1;i < argc;i++)
        {
            char* filename = argv[i];
            DirectoryEntry* dir = DirectoryFromName(filename);

            DeleteFile(*dir);
        }
    }
}