#pragma once
#include "Typedefs.h"
#include "TextMode.h"
#include "Memory/PageFrameAllocator.h"
#include "Drivers/Keyboard.h"
#include "ProcessManager.h"
#include "kmath.h"
#include "kstring.h"
#include "Utils.h"

extern char* input;
extern uint16 _index;
extern uint16 length;
extern uint64 cmdPos;

extern bool select;
extern uint16 start;

extern bool executing;

void InitializeCommandLine();
void StartCommandLine();

void ExecuteCommand(char* command);

uint64 NextWord();
uint64 PrevWord();

void ShiftUp(char* string, uint16 _index, char chr);
void ShiftDown(char* string, uint16 _index);

uint64 WordCount(const char* msg);
char** SplitWords(const char* msg, uint64 words);

void RenderInput();