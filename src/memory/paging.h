#pragma once
#include "../defs.h"
#include "pageframe.h"

#define PRESENT 0b000000000001
#define READ_WRITE 0b000000000010
#define USER_SUPERVISOR 0b000000000100
#define WRITE_THROUGH 0b000000001000
#define CACHE_DISABLED 0b000000010000
#define ACCESSED 0b000000100000
#define LARGER_PAGES 0b000010000000
#define OS1 0b001000000000
#define OS2 0b010000000000
#define OS3 0b100000000000

struct Page
{
    uint64_t value;

    bool get_flag(uint64_t flag)
    {
        return value & flag;
    }

    void set_flag(uint64_t flag, bool set)
    {
        value &= ~flag;
        if (set)
            value |= flag;
    }

    uint64_t get_address()
    {
        return (value & 0x000FFFFFFFFFF000) >> 12;
    }

    void set_address(uint64_t address)
    {
        address &= 0x000000FFFFFFFFFF;
        value &= 0xFFF0000000000FFF;
        value |= (address << 12);
    }
};

struct PageTable
{
    Page entries[512];

}__attribute__((aligned(0x1000)));

extern PageTable* pml4;

void paging_init();
void map_memory(void* virtual_address, void* physical_address);