#pragma once
#include "Typedefs.h"

void* memcpy(void* address, void* src, uint64 num);
void* memset(void* address, uint64 value, uint64 num);

char* strcpy(char* destination, char* source);
char* strncpy(char* destination, char* source, uint64 num);

char* strcat(char* destination, char* source);
char* strncat(char* destination, char* source, uint64 num);

int64 memcmp(void* ptr1, void* ptr2, uint64 num);
int64 strcmp(char* str1, char* str2);
int64 strncmp(char* str1, char* str2, uint64 num);

void* memchr(void* ptr, uint8 value, uint64 num);
char* strchr(char* str, char value);

uint64 strlen(char* string);

bool Match(char* str1, char* str2);