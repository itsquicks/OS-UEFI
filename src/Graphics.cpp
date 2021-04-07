#include "Graphics.h"

Framebuffer* framebuffer;
PSF2_FONT* font;

void PutPixel(uint32 x, uint32 y, uint32 color)
{
    uint32* address = (uint32*)((uint64)framebuffer->BaseAddress + 4 * x + 4 * framebuffer->PixelsPerScanLine * y);
    *address = color;
}

uint32 GetPixel(uint32 x, uint32 y)
{
    uint32* address = (uint32*)((uint64)framebuffer->BaseAddress + 4 * x + 4 * framebuffer->PixelsPerScanLine * y);
    return *address;
}

void DrawChar(uint32 x, uint32 y, uint8 chr, uint32 front, uint32 background)
{
    uint8* fontPtr = (uint8*)font->glyphBuffer + chr * font->header->charsize;

    for (uint32 _y = y; _y < y + font->header->height; _y++)
    {
        uint8 off = 0;

        for (uint32 _x = x; _x < x + font->header->width; _x++)
        {
            if (_x - x == 8)
            {
                off = 8;
                fontPtr++;
            }

            if ((*fontPtr & (0b10000000 >> (_x - x - off))) > 0)
                PutPixel(_x, _y, front);
            else
                PutPixel(_x, _y, background);
        }
        fontPtr++;
    }
}

void DrawCursor(uint32 x, uint32 y, uint32 color)
{
    for (uint32 _y = y + font->header->height - 2; _y < y + font->header->height; _y++)
    {
        for (uint32 _x = x; _x < x + font->header->width; _x++)
        {
            PutPixel(_x, _y, color);
        }
    }
}