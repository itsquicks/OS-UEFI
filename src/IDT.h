#pragma once
#include "Typedefs.h"
#include "TextMode.h"
#include "Drivers/PIC.h"
#include "Drivers/Keyboard.h"
#include "Syscalls.h"

struct IDT64
{
    uint16 offset_low;
    uint16 selector;
    uint8 ist;
    uint8 types_attr;
    uint16 offset_mid;
    uint32 offset_high;
    uint32 zero;
};

void SetIDT(uint64 isr, uint8 num);
void InitializeIDT();