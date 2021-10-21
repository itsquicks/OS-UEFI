#pragma once
#include "defs.h"
#include "memory/paging.h"
#include "utils.h"

struct Heap_Segment
{
    uint64_t size;
    Heap_Segment* next;
    Heap_Segment* last;
    bool free;

    void combine_forward();
    void combine_backward();
    Heap_Segment* split(uint64_t split_size);
};

extern Heap_Segment* last_seg;

void heap_init(void* address, uint64_t page_count);
void heap_expand(uint64_t size);

void* malloc(uint64_t size);
void* calloc(uint64_t size);
void free(void* address);

void print_heap();