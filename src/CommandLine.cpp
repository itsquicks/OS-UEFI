#include "CommandLine.h"

char* input;
uint16 _index;
uint16 length;
uint64 cmdPos;

bool select;
uint16 start;

bool executing = false;

void InitializeCommandLine()
{
    input = (char*)RequestPage();
    LockPage(input);
    memset(input, 0, 4096);
    _index = 0;
    length = 0;

    select = false;
    start = 0;

    PrintPath();
    colorf = RED;
    PrintChar('>');
    colorf = FRONT;
    cmdPos = cursorPos;
}

void StartCommandLine()
{
    while (1)
    {
        if (enter)
        {
            executing = true;

            enter = false;
            PrintString("\r\n");

            ExecuteCommand(input);

            input[0] = 0;
            _index = 0;
            length = 0;

            PrintPath();
            colorf = RED;
            PrintChar('>');
            colorf = FRONT;
            cmdPos = cursorPos;

            executing = false;
        }
    }
}

void ExecuteCommand(char* command)
{
    uint64 argc = WordCount(command);
    char** argv = SplitWords(command, argc);

    bool redirection = false;
    bool append = false;

    if (argc > 2)
    {
        if (Match(argv[argc - 2], ">"))
            redirection = true;
        else if (Match(argv[argc - 2], ">>"))
        {
            redirection = true;
            append = true;
        }
    }

    if (argc > 0)
    {
        if (Match(argv[0], "cls"))
        {
            cls();
        }
        else if (Match(argv[0], "memdmp"))
        {
            memdmp(argc, argv);
        }
        else if (Match(argv[0], "setbytes"))
        {
            setbytes(argc, argv);
        }
        else if (Match(argv[0], "ls"))
        {
            ls();
        }
        else if (Match(argv[0], "mem"))
        {
            mem();
        }
        else if (Match(argv[0], "load"))
        {
            load(argc, argv);
        }
        else if (Match(argv[0], "exec"))
        {
            exec(argc, argv);
        }
        else if (Match(argv[0], "create"))
        {
            create(argc, argv);
        }
        else if (Match(argv[0], "save"))
        {
            save(argc, argv);
        }
        else if (Match(argv[0], "del"))
        {
            del(argc, argv);
        }
        else
        {
            DirectoryEntry* program = DirectoryFromName(strcat(argv[0], ".exe"));

            if (program != 0)
            {
                void* programAddress = RequestPage();
                LockPage(programAddress);
                memset(programAddress, 0, 4096);

                LoadFile(*program, (uint64)programAddress);

                Process* process = BuildProcess(programAddress);

                StartProcess(process->PID, argc - (redirection ? 2 : 0), argv);

                if (!redirection)
                {
                    PrintString(GetProcessOutput(process->PID));
                    colorf = RED;
                    PrintString(GetProcessError(process->PID));
                    colorf = FRONT;
                }
                else
                {
                    char* filename = argv[argc - 1];

                    DirectoryEntry* dir = DirectoryFromName(filename);

                    if (dir == 0)
                    {
                        NewFile(filename, 0);
                        dir = DirectoryFromName(filename);
                        SaveFile(*dir, (uint64)GetProcessOutput(process->PID), 4096);
                    }
                }

                EndProcess(process->PID);
            }
            else
            {
                colorf = GRAY;
                PrintString("Unknown command.\r\n");
                colorf = FRONT;
            }
        }
    }

    for (int i = 0; i < argc; i++)
        FreePage(argv[i]);
    FreePage(argv);
}

uint64 NextWord()
{
    uint64 ret = _index;
    char* ptr = (char*)(input + _index);

    while (*ptr != 0)
    {
        if (*ptr == 0x20 && *(ptr + 1) != 0x20)
            return ret + 1;

        ptr++;
        ret++;
    }

    return ret;
}

uint64 PrevWord()
{
    uint64 ret = _index;
    char* ptr = (char*)(input + _index);

    while (ret != 0)
    {
        if (*ptr != 0x20 && *(ptr - 1) == 0x20 && ret != _index)
            return ret;

        ptr--;
        ret--;
    }

    return ret;
}

void ShiftUp(char* string, uint16 _index, char chr)
{
    uint16 i = strlen(string) + 1;
    while (i > _index)
    {
        *(string + i) = *(string + i - 1);
        i--;
    }
    *(string + _index) = chr;
}

void ShiftDown(char* string, uint16 _index)
{
    uint16 len = strlen(string);
    for (uint16 i = _index; i < len; i++)
    {
        *(string + i) = *(string + i + 1);
    }
}

uint64 WordCount(const char* msg)
{
    char* ptr = (char*)msg;
    uint8 count = 0;
    bool quotes = false;

    if (*ptr != 0x20 && *ptr != 0)
        count++;

    while (*ptr != 0)
    {

        if (*(ptr + 1) != 0)
        {
            if (!quotes)
            {
                if (*ptr == 0x20 && *(ptr + 1) != 0x20)
                {
                    count++;
                }
            }

            if (*(ptr + 1) == '"')
                quotes = !quotes;
        }
        else
        {
            return count;
        }
        ptr++;
    }

    return count;
}

char** SplitWords(const char* msg, uint64 words)
{
    char** argv = (char**)RequestPage();
    LockPage(argv);
    memset(argv, 0, 4096);
    for (uint64 i = 0; i < words; i++)
    {
        argv[i] = (char*)RequestPage();
        LockPage(argv[i]);
        memset(argv[i], 0, 4096);
    }

    char* ptr = (char*)msg;
    uint8 i = 0;
    uint8 j = 0;
    bool isEmpty = false;
    bool quotes = false;

    if (*ptr == 0x20)
        isEmpty = true;

    while (*ptr != 0)
    {
        if (!quotes)
        {
            if (*ptr == 0x20)
            {
                if (!isEmpty)
                {
                    *(argv[i] + j) = 0;
                    j = 0;
                    i++;

                }
                while (*ptr == 0x20 && *ptr != 0)
                    ptr++;

                if (isEmpty)
                    isEmpty = false;
                continue;
            }
        }

        if (*ptr == '"')
        {
            quotes = !quotes;
            ptr++;
            continue;
        }

        if (j < 31)
        {
            *(argv[i] + j) = *ptr;
            j++;
        }
        ptr++;
    }

    return argv;
}

void RenderInput()
{
    SetCursorPosition(cmdPos + _index);

    char* ptr = (char*)input;
    uint32 pos = cmdPos;

    uint64 left;
    uint64 right;
    bool inSelect;

    while (*ptr != 0)
    {
        left = min(start, _index);
        right = max(start, _index);
        inSelect = (pos >= cmdPos + left) && (pos < cmdPos + right) && select;

        textBuffer[pos] = { 0, *ptr,(uint32)(inSelect ? BACKGROUND : FRONT), (uint32)(inSelect ? FRONT : BACKGROUND) };

        pos++;
        ptr++;
    }

    left = min(start, _index);
    right = max(start, _index);
    inSelect = (pos >= cmdPos + left) && (pos < cmdPos + right) && select;

    textBuffer[pos] = { 0, 0x20,(uint32)(inSelect ? BACKGROUND : FRONT), (uint32)(inSelect ? FRONT : BACKGROUND) };

    RenderTextMode(cmdPos, cmdPos + length + 1);
    SetCursorPosition(cmdPos + _index);
}