#pragma once
#include "defs.h"
#include "utils.h"

struct Framebuffer
{
    void* base_address;
    uint64_t buffer_size;
    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
};

struct PSF2_Header
{
    uint8_t magic[4];
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t length;
    uint32_t char_size;
    uint32_t height;
    uint32_t width;
};

struct Font
{
    PSF2_Header* header;
    void* glyph_buffer;
};

extern Framebuffer* framebuffer;
extern Font* font;

void put_pixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t get_pixel(uint32_t x, uint32_t y);
void draw_char(uint64_t pos, uint8_t chr, uint32_t front, uint32_t background);
void draw_cursor(uint64_t pos, uint32_t color);