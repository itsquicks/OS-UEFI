#include "heap.h"

void* heap_start;
void* heap_end;
Heap_Segment* last_seg;

void heap_init(void* address, uint64_t page_count)
{
    void* ptr = address;

    for (uint64_t i = 0;i < page_count;i++)
    {
        map_memory(ptr, request_page());
        ptr = (void*)((uint64_t)ptr + 4096);
    }

    uint64_t heap_size = page_count * 4096;

    heap_start = address;
    heap_end = (void*)((uint64_t)heap_start + heap_size);

    Heap_Segment* start_seg = (Heap_Segment*)heap_start;

    start_seg->size = heap_size - sizeof(Heap_Segment);
    start_seg->next = null;
    start_seg->last = null;
    start_seg->free = true;

    last_seg = start_seg;
}

void Heap_Segment::combine_forward()
{
    if (next == null)
        return;

    if (!next->free)
        return;

    if (next == last_seg)
        last_seg = this;

    if (next->next != null)
        next->next->last = this;

    size += next->size + sizeof(Heap_Segment);
    next = next->next;
}

void Heap_Segment::combine_backward()
{
    if (last != null && last->free)
        last->combine_forward();
}

Heap_Segment* Heap_Segment::split(uint64_t split_size)
{
    if (split_size < 8)
        return null;

    int64_t split_seg_size = size - split_size - sizeof(Heap_Segment);

    if (split_seg_size < 8)
        return null;

    Heap_Segment* split_seg = (Heap_Segment*)((uint64_t)this + split_size + sizeof(Heap_Segment));

    if (next != null)
        next->last = split_seg;

    split_seg->next = next;
    next = split_seg;
    split_seg->last = this;
    split_seg->size = split_seg_size;
    split_seg->free = free;
    size = split_size;

    if (last_seg == this)
        last_seg = split_seg;

    return split_seg;
}

void heap_expand(uint64_t size)
{
    if (size % 4096 > 0)
    {
        size -= (size % 4096);
        size += 4096;
    }

    uint64_t page_count = size / 4096;
    Heap_Segment* new_seg = (Heap_Segment*)heap_end;

    for (uint64_t i = 0;i < page_count;i++)
    {
        map_memory(heap_end, request_page());
        heap_end = (void*)((uint64_t)heap_end + 4096);
    }

    new_seg->free = true;
    new_seg->last = last_seg;
    last_seg->next = new_seg;
    last_seg = new_seg;
    new_seg->next = null;
    new_seg->size = size - sizeof(Heap_Segment);
    new_seg->combine_backward();
}

void* malloc(uint64_t size)
{
    if (size == 0)
        return null;

    if (size % 8 > 0)
    {
        size -= (size % 8);
        size += 8;
    }

    Heap_Segment* current_seg = (Heap_Segment*)heap_start;

    while (true)
    {
        if (current_seg->free)
        {
            if (current_seg->size > size)
            {
                current_seg->split(size);
                current_seg->free = false;
                return (void*)(current_seg + 1);
            }

            if (current_seg->size == size)
            {
                current_seg->free = false;
                return (void*)(current_seg + 1);
            }
        }

        if (current_seg->next == null)
            break;

        current_seg = current_seg->next;
    }

    heap_expand(size);

    return malloc(size);
}

void* calloc(uint64_t size)
{
    void* ret = malloc(size);

    memset(ret, 0, size);

    return ret;
}

void free(void* address)
{
    Heap_Segment* seg = (Heap_Segment*)address - 1;
    seg->free = true;
    seg->combine_forward();
    seg->combine_backward();
}

void print_heap()
{
    printf("Heap size = %d bytes\n", (uint64_t)heap_end - (uint64_t)heap_start);

    Heap_Segment* seg = (Heap_Segment*)heap_start;

    while (true)
    {
        printf("Segment: {  ");
        colorf = seg->free ? GREEN : RED;
        printf("%s", seg->free ? "free" : "used");
        colorf = FRONT;
        printf(",  start = ");
        colorf = GREEN;
        printf("0x%x", seg);
        colorf = FRONT;
        printf(",  entry = ");
        colorf = GREEN;
        printf("0x%x", (void*)((uint64_t)seg + sizeof(Heap_Segment)));
        colorf = FRONT;
        printf(",  end = ");
        colorf = GREEN;
        printf("0x%x", (void*)((uint64_t)seg + seg->size + sizeof(Heap_Segment)));
        colorf = FRONT;
        printf(",  size = ");
        colorf = YELLOW;
        printf("%d", seg->size);
        colorf = FRONT;
        printf("  }\n");

        if (seg->next == null)
            break;

        seg = seg->next;
    }
}