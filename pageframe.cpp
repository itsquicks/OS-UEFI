#include "pageframe.h"

Bitmap pages;

uint64_t total_memory;
uint64_t free_memory;
uint64_t reserved_memory;
uint64_t used_memory;

void pageframe_init(void* map, uint64_t map_size, uint64_t descriptor_size)
{
    uint64_t entries = map_size / descriptor_size;

    void* free_mem_seg = null;
    uint64_t free_mem_seg_size = 0;

    for (uint64_t i = 0;i < entries;i++)
    {
        EFI_Descriptor* desc = (EFI_Descriptor*)((uint64_t)map + i * descriptor_size);
        total_memory += desc->num_pages * 4096;

        if (desc->type == 7)
        {
            if (desc->num_pages * 4096 > free_mem_seg_size)
            {
                free_mem_seg = desc->physical_address;
                free_mem_seg_size = desc->num_pages * 4096;
            }
        }
    }

    free_memory = total_memory;

    pages.size = total_memory / 4096 / 8 + 1;
    pages.buffer = (uint8_t*)free_mem_seg;
    memset(pages.buffer, 0, pages.size);
    reserve_pages(pages.buffer, pages.size / 4096 + 1);

    for (uint64_t i = 0;i < entries;i++)
    {
        EFI_Descriptor* desc = (EFI_Descriptor*)((uint64_t)map + i * descriptor_size);

        if (desc->type != 7)
            reserve_pages(desc->physical_address, desc->num_pages);
    }
}

uint64_t min_index = 0;
void* request_page()
{
    for (;min_index < pages.size * 8;min_index++)
    {
        if (pages[min_index]) continue;

        lock_page((void*)(min_index * 4096));
        return (void*)(min_index * 4096);
    }

    return null;
}

void free_page(void* address)
{
    uint64_t index = (uint64_t)address / 4096;

    if (pages[index])
    {
        if (pages.set(index, false))
        {
            free_memory += 4096;
            used_memory -= 4096;

            if (index < min_index)
                min_index = index;
        }
    }
}

void free_pages(void* address, uint64_t num)
{
    for (uint64_t i = 0;i < num;i++)
        free_page((void*)((uint64_t)address + i * 4096));
}

void lock_page(void* address)
{
    uint64_t index = (uint64_t)address / 4096;

    if (!pages[index])
    {
        if (pages.set(index, true))
        {
            free_memory -= 4096;
            used_memory += 4096;
        }
    }
}

void lock_pages(void* address, uint64_t num)
{
    for (uint64_t i = 0;i < num;i++)
        lock_page((void*)((uint64_t)address + i * 4096));
}

void unreserve_page(void* address)
{
    uint64_t index = (uint64_t)address / 4096;

    if (pages[index])
    {
        if (pages.set(index, false))
        {
            free_memory += 4096;
            reserved_memory -= 4096;

            if (index < min_index)
                min_index = index;
        }
    }
}

void unreserve_pages(void* address, uint64_t num)
{
    for (uint64_t i = 0;i < num;i++)
        unreserve_page((void*)((uint64_t)address + i * 4096));
}

void reserve_page(void* address)
{
    uint64_t index = (uint64_t)address / 4096;

    if (!pages[index])
    {
        if (pages.set(index, true))
        {
            free_memory -= 4096;
            reserved_memory += 4096;
        }
    }
}

void reserve_pages(void* address, uint64_t num)
{
    for (uint64_t i = 0;i < num;i++)
        reserve_page((void*)((uint64_t)address + i * 4096));
}