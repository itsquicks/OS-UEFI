#include "PageFrameAllocator.h"

Bitmap pages;

uint64 totalMemory;
uint64 freeMemory;
uint64 reservedMemory;
uint64 usedMemory;

void InitializePageFrameAllocator(void* map, uint64 mapSize, uint64 descriptorSize)
{
    uint64 entries = mapSize / descriptorSize;

    void* freeMemSeg = null;
    uint64 freeMemSegSize = 0;

    for (uint64 i = 0;i < entries;i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64)map + i * descriptorSize);
        totalMemory += desc->numPages * 4096;

        if (desc->type == 7)
        {
            if (desc->numPages * 4096 > freeMemSegSize)
            {
                freeMemSeg = desc->physicalAddress;
                freeMemSegSize = desc->numPages * 4096;
            }
        }
    }

    freeMemory = totalMemory;

    pages.size = totalMemory / 4096 / 8 + 1;
    pages.buffer = (uint8*)freeMemSeg;
    memset(pages.buffer, 0, pages.size);
    ReservePages(pages.buffer, pages.size / 4096 + 1);

    for (uint64 i = 0;i < entries;i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64)map + i * descriptorSize);

        if (desc->type != 7)
            ReservePages(desc->physicalAddress, desc->numPages);
    }
}

uint64 minIndex = 0;
void* RequestPage()
{
    for (;minIndex < pages.size * 8;minIndex++)
    {
        if (pages[minIndex]) continue;

        LockPage((void*)(minIndex * 4096));
        return (void*)(minIndex * 4096);
    }

    return null;
}

void FreePage(void* address)
{
    uint64 index = (uint64)address / 4096;

    if (pages[index])
    {
        if (pages.Set(index, false))
        {
            freeMemory += 4096;
            usedMemory -= 4096;

            if (index < minIndex)
                minIndex = index;
        }
    }
}

void FreePages(void* address, uint64 num)
{
    for (uint64 i = 0;i < num;i++)
        FreePage((void*)((uint64)address + i * 4096));
}

void LockPage(void* address)
{
    uint64 index = (uint64)address / 4096;

    if (!pages[index])
    {
        if (pages.Set(index, true))
        {
            freeMemory -= 4096;
            usedMemory += 4096;
        }
    }
}

void LockPages(void* address, uint64 num)
{
    for (uint64 i = 0;i < num;i++)
        LockPage((void*)((uint64)address + i * 4096));
}

void UnreservePage(void* address)
{
    uint64 index = (uint64)address / 4096;

    if (pages[index])
    {
        if (pages.Set(index, false))
        {
            freeMemory += 4096;
            reservedMemory -= 4096;

            if (index < minIndex)
                minIndex = index;
        }
    }
}

void UnreservePages(void* address, uint64 num)
{
    for (uint64 i = 0;i < num;i++)
        UnreservePage((void*)((uint64)address + i * 4096));
}

void ReservePage(void* address)
{
    uint64 index = (uint64)address / 4096;

    if (!pages[index])
    {
        if (pages.Set(index, true))
        {
            freeMemory -= 4096;
            reservedMemory += 4096;
        }
    }
}

void ReservePages(void* address, uint64 num)
{
    for (uint64 i = 0;i < num;i++)
        ReservePage((void*)((uint64)address + i * 4096));
}