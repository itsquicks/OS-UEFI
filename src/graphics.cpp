#include "graphics.h"

Framebuffer* framebuffer;
Font* font;

inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t* address = (uint32_t*)framebuffer->base_address + x + y * framebuffer->pixels_per_scanline;
    *address = color;
}

uint32_t get_pixel(uint32_t x, uint32_t y)
{
    uint32_t* address = (uint32_t*)framebuffer->base_address + x + y * framebuffer->pixels_per_scanline;
    return *address;
}

void draw_char(uint64_t pos, uint8_t chr, uint32_t front, uint32_t background)
{
    uint8_t* font_ptr = (uint8_t*)font->glyph_buffer + chr * font->header->char_size;

    uint32_t buffer[font->header->width];

    for (uint32_t h = 0; h < font->header->height;h++)
    {
        uint8_t off = 0;

        for (uint32_t w = 0; w < font->header->width; w++)
        {
            if (w == 8)
            {
                off = 8;
                font_ptr++;
            }

            buffer[w] = (*font_ptr & (0b10000000 >> (w - off))) ? front : background;
        }
        font_ptr++;

        memcpy((uint32_t*)framebuffer->base_address + pos + h * framebuffer->width, buffer, font->header->width * sizeof(uint32_t));
    }
}

void draw_cursor(uint64_t pos, uint32_t color)
{
    memset((uint32_t*)framebuffer->base_address + pos, color * 0x100000001, font->header->width * sizeof(uint32_t));
    memset((uint32_t*)framebuffer->base_address + pos + framebuffer->pixels_per_scanline, color * 0x100000001, font->header->width * sizeof(uint32_t));
}