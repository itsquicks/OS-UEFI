#pragma once
#include "../Typedefs.h"
#include "../IO.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xa0
#define PIC2_DATA 0xa1

#define PIC_EOI	0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void PIC_Remap();
void PIC_SetMask1(uint8 mask);
void PIC_SetMask2(uint8 mask);
void PIC_SendEOI(uint8 irq);