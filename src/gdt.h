#pragma once
#include "defs.h"

struct GDT_Descriptor
{
    uint16_t size;
    uint64_t offset;

} __attribute__((packed));

struct GDT_Entry
{
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access_byte;
    uint8_t limit1_flags;
    uint8_t base2;

}__attribute__((packed));

struct GDT
{
    GDT_Entry kernel_null;
    GDT_Entry kernel_code;
    GDT_Entry kernel_data;
    GDT_Entry user_null;
    GDT_Entry user_code;
    GDT_Entry user_data;

} __attribute__((packed)) __attribute((aligned(0x1000)));

extern GDT gdt;

extern "C" void gdt_load(GDT_Descriptor * gdt_descriptor);

void gdt_init();