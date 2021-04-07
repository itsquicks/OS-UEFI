#pragma once
#include "Typedefs.h"
#include "Graphics.h"
#include "kconvert.h"
#include "kstring.h"
#include <stdarg.h>
#include "Memory/PageFrameAllocator.h"
#include "CommandLine.h"

struct TextSlot
{
    uint8 zero;
    char chr;
    uint32 color : 24;
    uint32 background : 24;
}__attribute__((packed));

extern TextSlot* textBuffer;
extern uint32 cursorPos;
extern uint32 colorf;
extern uint32 colorb;
extern uint8 textModeWidth;
extern uint8 textModeHeight;

void InitializeTextMode();

void SetCursorPosition(uint32 pos);
void ClearScreen();
void Scroll();
void PrintString(char* str);
void PrintBackString(char* string);
void PrintChar(char chr, bool move = true);

void RenderTextMode();
void RenderTextMode(uint64 index);
void RenderTextMode(uint64 start, uint64 end);