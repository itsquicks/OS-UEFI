#pragma once
#include "../Typedefs.h"
#include "EFI_Memory.h"
#include "../kstring.h"

struct Bitmap
{
    uint64 size;
    uint8* buffer;

    bool operator[](uint64 index)
    {
        if (index > size * 8) return false;

        uint64 byteIndex = index / 8;
        uint8 bitIndex = index % 8;
        uint8 selector = 0b10000000 >> bitIndex;

        return buffer[byteIndex] & selector;
    }

    bool Set(uint64 index, bool value)
    {
        if (index > size * 8) return false;

        uint64 byteIndex = index / 8;
        uint8 bitIndex = index % 8;
        uint8 selector = 0b10000000 >> bitIndex;

        buffer[byteIndex] &= ~selector;

        if (value)
            buffer[byteIndex] |= selector;

        return true;
    }
};

extern Bitmap pages;

extern uint64 totalMemory;
extern uint64 freeMemory;
extern uint64 reservedMemory;
extern uint64 usedMemory;

void InitializePageFrameAllocator(void* map, uint64 mapSize, uint64 descriptorSize);

void* RequestPage();

void FreePage(void* address);
void FreePages(void* address, uint64 num);
void LockPage(void* address);
void LockPages(void* address, uint64 num);

void UnreservePage(void* address);
void UnreservePages(void* address, uint64 num);
void ReservePage(void* address);
void ReservePages(void* address, uint64 num);


