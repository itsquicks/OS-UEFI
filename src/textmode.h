#pragma once
#include "defs.h"
#include "graphics.h"
#include "utils.h"
#include "heap.h"
#include "shell.h"

struct TextSlot
{
    uint8_t zero;
    char chr;
    uint32_t color : 24;
    uint32_t background : 24;

}__attribute__((packed));

extern TextSlot* text_buffer;
extern uint32_t cursor_position;
extern uint32_t colorf;
extern uint32_t colorb;
extern uint8_t textmode_width;
extern uint8_t textmode_height;

void textmode_init();

void textmode_set_cursor(uint32_t pos);
void textmode_clear_screen();
void textmode_scroll();
void print_string(char* str);
void print_char(char chr, bool move = true);

void textmode_render();
void textmode_render(uint64_t index);
void textmode_render(uint64_t start, uint64_t end);