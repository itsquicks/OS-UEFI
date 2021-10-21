#pragma once
#include "../defs.h"
#include "../shell.h"
#include "../math.h"
#include "../utils.h"

const char scan_code_lookup[] =
{
    0, 0, '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=', 0, 0,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
    0, 0, 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/', 0, '*',
    0, ' '
};

extern bool enter;
extern bool lshift;
extern bool rshift;
extern bool caps;
extern bool ctrl;

void simple_keyboard_handler(uint8_t scan_code, uint8_t chr);
void delete_selection();