#include "Keyboard.h"

bool enter = false;
bool lshift = false;
bool rshift = false;
bool caps = false;
bool ctrl = false;

uint8 lastScanCode;

void SimpleKeyboardHandler(uint8 scanCode, uint8 chr)
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
            select = true;
            start = 0;
            _index = length;
        }
        else
        {
            if (select)
                DeleteSelection();

            if (length + 1 <= 256)
            {
                ShiftUp(input, _index, chr);
                _index++;
                length++;
            }
        }

        RenderInput();
    }
    else
    {
        switch (scanCode)
        {
        case 0x0e:                                     // Backspace
            if (select)
                DeleteSelection();
            else
            {
                if (_index > 0)
                {
                    ShiftDown(input, _index - 1);
                    _index--;
                    length--;
                }
            }
            RenderInput();
            break;
        case 0x53:                                  // Delete
            if (_index != length)
            {
                ShiftDown(input, _index);
                length--;
                RenderInput();
            }
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
            select = false;
            RenderInput();
            break;
        case 0x9c:                                  // Enter Released
            enter = false;
            break;
            //case 0x48:                            // Up arrow
            //case 0x50:                            // Down arrow
        case 0x4b:                                  // Left arrow
            if (!lshift && !rshift && select)
            {
                select = false;
                _index = min(start, _index);
            }
            else if (_index > 0)
            {
                if ((lshift || rshift) && !select)
                {
                    select = true;
                    start = _index;
                }

                if (ctrl)
                    _index = PrevWord();
                else
                    _index--;
            }
            RenderInput();
            break;
        case 0x4d:                                  //Right arrow
            if (!lshift && !rshift && select)
            {
                select = false;
                _index = max(start, _index);
            }
            else if (_index < length)
            {
                if ((lshift || rshift) && !select)
                {
                    select = true;
                    start = _index;
                }

                if (ctrl)
                    _index = NextWord();
                else
                    _index++;
            }
            RenderInput();
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

    lastScanCode = scanCode;
}

void DeleteSelection()
{
    select = false;
    RenderInput();

    uint16 left = min(start, _index);
    int size = abs((int)start - (int)_index);

    _index = left;
    SetCursorPosition(cmdPos + _index);

    while (size > 0)
    {
        if (input[left] == 0)
            break;

        ShiftDown(input, left);
        size--;
        length--;

        RenderInput();
    }
}