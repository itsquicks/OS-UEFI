#include "textmode.h"

TextSlot* text_buffer;
uint32_t cursor_position = 0;
uint32_t colorf = FRONT;
uint32_t colorb = BACKGROUND;
uint8_t textmode_width;
uint8_t textmode_height;

void textmode_init()
{
    textmode_width = (framebuffer->width / font->header->width);
    textmode_height = (framebuffer->height / font->header->height);

    text_buffer = (TextSlot*)malloc(textmode_height * textmode_width * sizeof(TextSlot));

    textmode_clear_screen();

    textmode_set_cursor(0);
}

void textmode_set_cursor(uint32_t pos)
{
    if (pos < textmode_height * textmode_width)
    {
        textmode_render(cursor_position);

        cursor_position = pos;

        uint32_t x = (pos % textmode_width) * font->header->width;
        uint32_t y = (pos / textmode_width) * font->header->height + font->header->height - 2;

        draw_cursor(x + y * framebuffer->width, text_buffer[pos].color);
    }
    else
    {
        textmode_set_cursor(pos - textmode_width);
        textmode_scroll();
    }
}

void textmode_clear_screen()
{
    uint64_t value = ((uint64_t)BACKGROUND << 40) + ((uint64_t)FRONT << 16) + 0x2000;

    memset(text_buffer, value, textmode_height * textmode_width * sizeof(TextSlot));
    memset(framebuffer->base_address, BACKGROUND * 0x100000001, framebuffer->width * framebuffer->height * sizeof(uint32_t));
}

void textmode_scroll()
{
    textmode_render(cursor_position);

    uint64_t fb_end = (framebuffer->height - font->header->height) * framebuffer->width;
    memcpy(framebuffer->base_address, (uint32_t*)framebuffer->base_address + framebuffer->width * font->header->height, fb_end * sizeof(uint32_t));

    uint64_t tb_end = (textmode_height - 1) * textmode_width;
    memcpy(text_buffer, text_buffer + textmode_width, tb_end * sizeof(TextSlot));

    uint64_t value = ((uint64_t)BACKGROUND << 40) + ((uint64_t)FRONT << 16) + 0x2000;

    memset(text_buffer + tb_end, value, textmode_width * sizeof(TextSlot));
    memset((uint32_t*)framebuffer->base_address + fb_end, BACKGROUND * 0x100000001, framebuffer->width * font->header->height * sizeof(uint32_t));

    cmd_position -= textmode_width;
}

void print_string(char* string)
{
    char* ptr = (char*)string;

    while (*ptr != 0)
    {
        print_char(*ptr);
        ptr++;
    }
}

void print_char(char chr, bool move)
{
    switch (chr)
    {
    case '\t':
        textmode_set_cursor(cursor_position + 4 - (cursor_position % textmode_width) % 4);
        break;
    case '\v':
        textmode_set_cursor(cursor_position + textmode_width);
        break;
    case '\r':
        textmode_set_cursor(cursor_position - cursor_position % textmode_width);
        break;
    case '\n':
        textmode_set_cursor(cursor_position - cursor_position % textmode_width + textmode_width);
        break;
    default:
        text_buffer[cursor_position] = { 0, chr,colorf,colorb };
        textmode_render(cursor_position);
        textmode_set_cursor(cursor_position + move);
        break;
    }
}

void textmode_render()
{
    /*for (uint64_t i = 0; i < textmode_height * textmode_width;i++)
    {
        uint32_t x = (i % textmode_width) * font->header->width;
        uint32_t y = (i / textmode_width) * font->header->height;

        draw_char(x + y * 1920, text_buffer[i].chr, text_buffer[i].color, text_buffer[i].background);
    }*/

    /*uint8_t* font_ptr = (uint8_t*)font->_blyphBuffer + chr * font->header->charsize;

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

        memcpy((uint32_t*)((uint64_t)framebuffer->base_address) + pos + h * 1920, buffer, font->header->width * 4);
    }*/



    uint32_t buffer[1920];

    for (uint32_t j = 0;j < textmode_height;j++)
    {
        for (uint32_t h = 0;h < font->header->height;h++)
        {
            for (int i = 0;i < textmode_width;i++)
            {
                uint8_t* font_ptr = (uint8_t*)font->glyph_buffer + text_buffer[i + j * textmode_width].chr * font->header->char_size;
                uint8_t off = 0;

                font_ptr += 2 * h;

                for (uint32_t w = 0; w < font->header->width; w++)
                {
                    if (w == 8)
                    {
                        off = 8;
                        font_ptr++;
                    }

                    if (*font_ptr & (0b10000000 >> (w - off)))
                        buffer[w + i * font->header->width] = text_buffer[i + j * textmode_width].color;
                    else
                        buffer[w + i * font->header->width] = text_buffer[i + j * textmode_width].background;
                }
            }

            memcpy((uint32_t*)framebuffer->base_address + (h + j * font->header->height) * 1920, buffer, sizeof(buffer));
        }
    }
}

void textmode_render(uint64_t index)
{
    uint32_t x = (index % textmode_width) * font->header->width;
    uint32_t y = (index / textmode_width) * font->header->height;

    draw_char(x + y * framebuffer->width, text_buffer[index].chr, text_buffer[index].color, text_buffer[index].background);
}

void textmode_render(uint64_t start, uint64_t end)
{
    for (uint64_t i = start; i < end;i++)
    {
        uint32_t x = (i % textmode_width) * font->header->width;
        uint32_t y = (i / textmode_width) * font->header->height;

        draw_char(x + y * framebuffer->width, text_buffer[i].chr, text_buffer[i].color, text_buffer[i].background);
    }
}


