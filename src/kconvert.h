#pragma once
#include "Typedefs.h"

char* itos(int64 value);
char* uitos(uint64 value);
char* dtos(double value, uint8 decimals = 5);
char* xtos(uint8 value);
char* xtos(uint16 value);
char* xtos(uint32 value);
char* xtos(uint64 value);
uint64 stoi(char* string);
uint64 stox(char* string);




