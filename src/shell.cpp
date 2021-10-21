#include "shell.h"

char* input;
uint16_t input_index;
uint16_t length;
uint64_t cmd_position;

bool selection;
uint16_t start;

bool executing = false;

void shell_init()
{
    input = (char*)calloc(512);
    input_index = 0;
    length = 0;

    selection = false;
    start = 0;

    print_path();
    colorf = RED;
    print_char('>');
    colorf = FRONT;
    cmd_position = cursor_position;
}

void shell_start()
{
    while (1)
    {
        if (enter)
        {
            executing = true;

            enter = false;
            print_string("\r\n");

            shell_execute(input);

            input[0] = 0;
            input_index = 0;
            length = 0;

            print_path();
            colorf = RED;
            print_char('>');
            colorf = FRONT;
            cmd_position = cursor_position;

            executing = false;
        }
    }
}

void shell_execute(char* command)
{
    uint64_t argc = word_count(command);
    char** argv = split_words(command, argc);

    if (argc > 0)
    {
        if (match(argv[0], "cls"))
        {
            cls();
        }
        else if (match(argv[0], "info"))
        {
            info();
        }
        else if (match(argv[0], "load"))
        {
            load(argc, argv);
        }
        else if (match(argv[0], "create"))
        {
            create(argc, argv);
        }
        else if (match(argv[0], "write"))
        {
            write(argc, argv);
        }
        else if (match(argv[0], "ls"))
        {
            ls();
        }
        else if (match(argv[0], "cat"))
        {
            cat(argc, argv);
        }
        else if (match(argv[0], "cd"))
        {
            cd(argc, argv);
        }
        else if (match(argv[0], "sb"))
        {
            sb(argc, argv);
        }
        else if (match(argv[0], "memdmp"))
        {
            memdmp(argc, argv);
        }
        else if (match(argv[0], "memcpy"))
        {
            memcpy_app(argc, argv);
        }
        else if (match(argv[0], "memset"))
        {
            memset_app(argc, argv);
        }
        else if (match(argv[0], "exec"))
        {
            exec(argc, argv);
        }
        else if (match(argv[0], "inode"))
        {
            inode_app(argc, argv);
        }
        else if (match(argv[0], "remove"))
        {
            remove(argc, argv);
        }
        else if (match(argv[0], "mkdir"))
        {
            mkdir(argc, argv);
        }
        else if (match(argv[0], "rmdir"))
        {
            rmdir(argc, argv);
        }
        else if (match(argv[0], "heap"))
        {
            heap();
        }
        else
        {
            colorf = GRAY;
            print_string("Unknown command.\r\n");
            colorf = FRONT;
        }
    }

    for (int i = 0; i < argc; i++)
        free(argv[i]);

    free(argv);
}

uint64_t next_word()
{
    uint64_t ret = input_index;
    char* ptr = (char*)(input + input_index);

    while (*ptr != 0)
    {
        if (*ptr == 0x20 && *(ptr + 1) != 0x20)
            return ret + 1;

        ptr++;
        ret++;
    }

    return ret;
}

uint64_t prev_word()
{
    uint64_t ret = input_index;
    char* ptr = (char*)(input + input_index);

    while (ret != 0)
    {
        if (*ptr != 0x20 && *(ptr - 1) == 0x20 && ret != input_index)
            return ret;

        ptr--;
        ret--;
    }

    return ret;
}

void shift_up(char* string, uint16_t input_index, char chr)
{
    uint16_t i = strlen(string) + 1;
    while (i > input_index)
    {
        *(string + i) = *(string + i - 1);
        i--;
    }
    *(string + input_index) = chr;
}

void shift_down(char* string, uint16_t input_index)
{
    uint16_t len = strlen(string);
    for (uint16_t i = input_index; i < len; i++)
    {
        *(string + i) = *(string + i + 1);
    }
}

uint64_t word_count(const char* msg)
{
    char* ptr = (char*)msg;
    uint8_t count = 0;
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

char** split_words(const char* msg, uint64_t words)
{
    char** argv = (char**)calloc(64);

    for (uint64_t i = 0; i < words; i++)
        argv[i] = (char*)calloc(256);

    char* ptr = (char*)msg;
    uint8_t i = 0;
    uint8_t j = 0;
    bool empty = false;
    bool quotes = false;

    if (*ptr == 0x20)
        empty = true;

    while (*ptr != 0)
    {
        if (!quotes)
        {
            if (*ptr == 0x20)
            {
                if (!empty)
                {
                    *(argv[i] + j) = 0;
                    j = 0;
                    i++;

                }
                while (*ptr == 0x20 && *ptr != 0)
                    ptr++;

                if (empty)
                    empty = false;
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

void render_input()
{
    textmode_set_cursor(cmd_position + input_index);

    char* ptr = (char*)input;
    uint32_t pos = cmd_position;

    uint64_t left;
    uint64_t right;
    bool inselection;

    while (*ptr != 0)
    {
        left = min(start, input_index);
        right = max(start, input_index);
        inselection = (pos >= cmd_position + left) && (pos < cmd_position + right) && selection;

        text_buffer[pos] = { 0, *ptr,(uint32_t)(inselection ? BACKGROUND : FRONT), (uint32_t)(inselection ? FRONT : BACKGROUND) };

        pos++;
        ptr++;
    }

    text_buffer[pos] = { 0, 0x20,FRONT, BACKGROUND };

    textmode_render(cmd_position, cmd_position + length + 1);
    textmode_set_cursor(cmd_position + input_index);
}