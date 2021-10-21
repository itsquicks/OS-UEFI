#pragma once
#include "../defs.h"
#include "../io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xa0
#define PIC2_DATA 0xa1

#define PIC_EOI	0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_remap();
void pic_set_mask1(uint8_t mask);
void pic_set_mask2(uint8_t mask);
void pic_send_EOI(uint8_t irq);