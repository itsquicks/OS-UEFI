#pragma once
#include "defs.h"
#include "textmode.h"
#include "heap.h"
#include "drivers/keyboard.h"
#include "math.h"
#include "utils.h"
#include "core_apps.h"
#include "ext2.h"

extern char* input;
extern uint16_t input_index;
extern uint16_t length;
extern uint64_t cmd_position;

extern bool selection;
extern uint16_t start;

extern bool executing;

void shell_init();
void shell_start();

void shell_execute(char* command);

uint64_t next_word();
uint64_t prev_word();

void shift_up(char* string, uint16_t _index, char chr);
void shift_down(char* string, uint16_t _index);

uint64_t word_count(const char* msg);
char** split_words(const char* msg, uint64_t words);

void render_input();