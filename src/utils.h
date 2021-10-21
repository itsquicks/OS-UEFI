#pragma once
#include "defs.h"
#include "textmode.h"

#include "stdarg.h"
//#include <immintrin.h>

//void* memcpy_256bit_512B_u(void* dest, const void* src, size_t len);
//void* memcpy_256bit_u(void* dest, const void* src, size_t len);

void* memcpy(void* address, void* src, uint64_t num);
void* memset(void* address, uint64_t value, uint64_t num);
void* memsetbyte(void* address, uint8_t value, uint64_t num);

char* strcpy(char* destination, char* source);
char* strncpy(char* destination, char* source, uint64_t num);

char* strcat(char* destination, char* source);
char* strncat(char* destination, char* source, uint64_t num);

int64_t memcmp(void* ptr1, void* ptr2, uint64_t num);
int64_t strcmp(char* str1, char* str2);
int64_t strncmp(char* str1, char* str2, uint64_t num);

void* memchr(void* ptr, uint8_t value, uint64_t num);
char* strchr(char* str, char value);

uint64_t strlen(char* string);

void printf(char* format, ...);

void print_buffer(uint8_t* buffer, uint64_t size);

char* itos(int64_t value);
char* uitos(uint64_t value);
char* dtos(double value, uint8_t decimals = 2);
char* xtos(uint8_t value);
char* xtos(uint16_t value);
char* xtos(uint32_t value);
char* xtos(uint64_t value);
uint64_t stoi(char* string);
uint64_t stox(char* string);

bool match(char* str1, char* str2);




