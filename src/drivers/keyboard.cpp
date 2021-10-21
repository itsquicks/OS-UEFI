#include "keyboard.h"

bool enter = false;
bool lshift = false;
bool rshift = false;
bool caps = false;
bool ctrl = false;

uint8_t last_scan_code;

void simple_keyboard_handler(uint8_t scan_code, uint8_t chr)
{
    if (executing)
        return;

    if (lshift || rshift || caps)
    {
        if (chr >= 'a' && chr <= 'z')
        {
            chr -= 32;
        }
        else if (lshift || rshift)
        {
            switch (chr)
            {
            case '`':
                chr = '~';
                break;
            case '1':
                chr = '!';
                break;
            case '2':
                chr = '@';
                break;
            case '3':
                chr = '#';
                break;
            case '4':
                chr = '$';
                break;
            case '5':
                chr = '%';
                break;
            case '6':
                chr = '^';
                break;
            case '7':
                chr = '&';
                break;
            case '8':
                chr = '*';
                break;
            case '9':
                chr = '(';
                break;
            case '0':
                chr = ')';
                break;
            case '-':
                chr = '_';
                break;
            case '=':
                chr = '+';
                break;
            case '[':
                chr = '{';
                break;
            case ']':
                chr = '}';
                break;
            case ';':
                chr = ':';
                break;
            case 0x27:
                chr = 0x22;
                break;
            case 0x5c:
                chr = 0x7c;
                break;
            case ',':
                chr = '<';
                break;
            case '.':
                chr = '>';
                break;
            case '/':
                chr = '?';
                break;
            }
        }
    }

    if (chr != 0)
    {
        if (ctrl && (chr == 'a' || chr == 'A'))
        {
            selection = true;
            start = 0;
            input_index = length;
        }
        else
        {
            if (selection)
                delete_selection();

            if (length + 1 <= 512)
            {
                shift_up(input, input_index, chr);
                input_index++;
                length++;
            }
        }

        render_input();
    }
    else
    {
        switch (scan_code)
        {
        case 0x0e:                                  // Backspace
            if (selection)
                delete_selection();
            else
            {
                if (input_index > 0)
                {
                    shift_down(input, input_index - 1);
                    input_index--;
                    length--;
                }
            }
            render_input();
            break;
        case 0x53:                                  // Delete
            if (selection)
                delete_selection();
            else
            {
                if (input_index != length)
                {
                    shift_down(input, input_index);
                    length--;
                }
            }
            render_input();
            break;
        case 0x2a:                                  // Left Shift Pressed
            lshift = true;
            break;
        case 0xaa:                                  // Left Shift Released
            lshift = false;
            break;
        case 0x36:                                  // Right Shift Pressed
            rshift = true;
            break;
        case 0xb6:                                  // Right Shift Released
            rshift = false;
            break;
        case 0x1c:                                  // Enter Pressed
            enter = true;
            selection = false;
            render_input();
            break;
        case 0x9c:                                  // Enter Released
            enter = false;
            break;
            //case 0x48:                            // Up arrow
            //case 0x50:                            // Down arrow
        case 0x4b:                                  // Left arrow
            if (!lshift && !rshift && selection)
            {
                selection = false;
                input_index = min(start, input_index);
            }
            else if (input_index > 0)
            {
                if ((lshift || rshift) && !selection)
                {
                    selection = true;
                    start = input_index;
                }

                if (ctrl)
                    input_index = prev_word();
                else
                    input_index--;
            }
            render_input();
            break;
        case 0x4d:                                  //Right arrow
            if (!lshift && !rshift && selection)
            {
                selection = false;
                input_index = max(start, input_index);
            }
            else if (input_index < length)
            {
                if ((lshift || rshift) && !selection)
                {
                    selection = true;
                    start = input_index;
                }

                if (ctrl)
                    input_index = next_word();
                else
                    input_index++;
            }
            render_input();
            break;
        case 0x3a:                                  // Caps Lock
            caps = !caps;
            break;
        case 0x1d:                                  // Control down
            ctrl = true;
            break;
        case 0x9d:                                   // Control up
            ctrl = false;
            break;
        }
    }

    last_scan_code = scan_code;
}

void delete_selection()
{
    selection = false;

    uint16_t left = min(start, input_index);
    int size = abs((int)start - (int)input_index);

    input_index = left;
    textmode_set_cursor(cmd_position + input_index);

    while (size > 0)
    {
        if (input[left] == 0)
            break;

        shift_down(input, left);
        size--;
        length--;

        text_buffer[cmd_position + length + 1] = { 0, 0x20, FRONT, BACKGROUND };
        textmode_render(cmd_position + length + 1);
    }
}