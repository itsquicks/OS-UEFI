#pragma once
#include "../defs.h"
#include "efi_memory.h"
#include "../utils.h"

extern Bitmap pages;

extern uint64_t total_memory;
extern uint64_t free_memory;
extern uint64_t reserved_memory;
extern uint64_t used_memory;

void pageframe_init(void* map, uint64_t map_size, uint64_t descriptor_size);

void* request_page();

void free_page(void* address);
void free_pages(void* address, uint64_t num);
void lock_page(void* address);
void lock_pages(void* address, uint64_t num);

void unreserve_page(void* address);
void unreserve_pages(void* address, uint64_t num);
void reserve_page(void* address);
void reserve_pages(void* address, uint64_t num);


