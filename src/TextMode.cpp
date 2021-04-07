#include "TextMode.h"

TextSlot* textBuffer;
uint32 cursorPos = 0;
uint32 colorf = FRONT;
uint32 colorb = BACKGROUND;
uint8 textModeWidth;
uint8 textModeHeight;

void InitializeTextMode()
{
    textModeWidth = (framebuffer->Width / font->header->width);
    textModeHeight = (framebuffer->Height / font->header->height);

    textBuffer = (TextSlot*)RequestPage();
    LockPages(textBuffer, (textModeHeight * textModeWidth * sizeof(TextSlot)) / 4096 + 1);

    ClearScreen();

    SetCursorPosition(0);
}

void SetCursorPosition(uint32 pos)
{
    if (pos < textModeHeight * textModeWidth)
    {
        RenderTextMode(cursorPos);

        cursorPos = pos;

        uint32 x = (pos % textModeWidth) * font->header->width;
        uint32 y = (pos / textModeWidth) * font->header->height;

        DrawCursor(x, y, textBuffer[pos].color);
    }
    else
    {
        SetCursorPosition(pos - textModeWidth);
        Scroll();
    }
}

void ClearScreen()
{
    for (uint64 i = 0;i < textModeHeight * textModeWidth;i++)
        textBuffer[i] = { 0, 0x20,FRONT,BACKGROUND };

    RenderTextMode();
}

void Scroll()
{
    RenderTextMode(cursorPos);

    uint64 size = framebuffer->Width * (framebuffer->Height - font->header->height) * sizeof(uint32);
    memcpy(framebuffer->BaseAddress, (uint64*)((uint64)framebuffer->BaseAddress + framebuffer->Width * sizeof(uint32) * font->header->height), size);

    memcpy(textBuffer, textBuffer + textModeWidth, textModeHeight * textModeWidth * sizeof(TextSlot));

    for (uint64 i = textModeWidth * (textModeHeight - 1);i < textModeHeight * textModeWidth;i++)
    {
        textBuffer[i] = { 0, 0x20,FRONT,BACKGROUND };
        RenderTextMode(i);
    }

    cmdPos -= textModeWidth;
}

void PrintString(char* string)
{
    char* ptr = (char*)string;

    while (*ptr != 0)
    {
        PrintChar(*ptr);
        ptr++;
    }
}

void PrintBackString(char* string)
{
    SetCursorPosition(cursorPos - strlen(string));
    PrintString(string);
}

void PrintChar(char chr, bool move)
{
    switch (chr)
    {
    case '\t':
        SetCursorPosition(cursorPos + 4 - (cursorPos % textModeWidth) % 4);
        break;
    case '\v':
        SetCursorPosition(cursorPos + textModeWidth);
        break;
    case '\r':
        SetCursorPosition(cursorPos - cursorPos % textModeWidth);
        break;
    case '\n':
        SetCursorPosition(cursorPos - cursorPos % textModeWidth + textModeWidth);
        break;
    default:
        textBuffer[cursorPos] = { 0, chr,colorf,colorb };
        RenderTextMode(cursorPos);
        SetCursorPosition(cursorPos + move);
        break;
    }
}

void RenderTextMode()
{
    for (uint64 i = 0; i < textModeHeight * textModeWidth;i++)
    {
        uint32 x = (i % textModeWidth) * font->header->width;
        uint32 y = (i / textModeWidth) * font->header->height;

        DrawChar(x, y, textBuffer[i].chr, textBuffer[i].color, textBuffer[i].background);
    }
}

void RenderTextMode(uint64 index)
{
    uint32 x = (index % textModeWidth) * font->header->width;
    uint32 y = (index / textModeWidth) * font->header->height;

    DrawChar(x, y, textBuffer[index].chr, textBuffer[index].color, textBuffer[index].background);
}

void RenderTextMode(uint64 start, uint64 end)
{
    for (uint64 i = start; i < end;i++)
    {
        uint32 x = (i % textModeWidth) * font->header->width;
        uint32 y = (i / textModeWidth) * font->header->height;

        DrawChar(x, y, textBuffer[i].chr, textBuffer[i].color, textBuffer[i].background);
    }
}


