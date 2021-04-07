#pragma once
#include "Typedefs.h"

struct Framebuffer
{
    void* BaseAddress;
    uint64 BufferSize;
    uint32 Width;
    uint32 Height;
    uint32 PixelsPerScanLine;
};

struct PSF2_HEADER
{
    unsigned char magic[4];
    unsigned int version;
    unsigned int headersize;
    unsigned int flags;
    unsigned int length;
    unsigned int charsize;
    unsigned int height, width;
};

struct PSF2_FONT
{
    PSF2_HEADER* header;
    void* glyphBuffer;
};

extern Framebuffer* framebuffer;
extern PSF2_FONT* font;

void PutPixel(uint32 x, uint32 y, uint32 color);
uint32 GetPixel(uint32 x, uint32 y);
void DrawChar(uint32 x, uint32 y, uint8 chr, uint32 front, uint32 background);
void DrawCursor(uint32 x, uint32 y, uint32 color);