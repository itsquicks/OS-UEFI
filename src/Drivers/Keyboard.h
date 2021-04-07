#pragma once
#include "../Typedefs.h"
#include "../CommandLine.h"
#include "../kmath.h"

const char ScanCodeLookupTable[] =
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

void SimpleKeyboardHandler(uint8 scanCode, uint8 chr);
void DeleteSelection();