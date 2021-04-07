#pragma once
#include "Typedefs.h"

struct GDTDescriptor
{
    uint16 size;
    uint64 offset;

} __attribute__((packed));

struct GDTEntry
{
    uint16 limit0;
    uint16 base0;
    uint8 base1;
    uint8 accessByte;
    uint8 limit1_flags;
    uint8 base2;

}__attribute__((packed));

struct GDT
{
    GDTEntry KernelNull;
    GDTEntry KernelCode;
    GDTEntry KernelData;
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;

} __attribute__((packed)) __attribute((aligned(0x1000)));

extern GDT gdt;

extern "C" void LoadGDT(GDTDescriptor * gdtDescriptor);