#pragma once
#include "defs.h"
#include "textmode.h"
#include "drivers/pic.h"
#include "drivers/keyboard.h"

struct IDT
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t types_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

void idt_set(uint64_t isr, uint8_t num);
void idt_init();