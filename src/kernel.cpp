#include "defs.h"
#include "gdt.h"
#include "idt.h"
#include "graphics.h"
#include "textmode.h"
#include "drivers/pci.h"
#include "drivers/cpu.h"
#include "memory/pageframe.h"
#include "memory/paging.h"
#include "heap.h"
#include "shell.h"
#include "disk.h"
#include "ext2.h"

struct BootInfo
{
	Framebuffer* framebuffer;
	Font* font;
	void* map;
	uint64_t map_size;
	uint64_t descriptor_size;
};

extern uint64_t _kernelStart_;
extern uint64_t _kernelEnd_;

extern "C" void _start(BootInfo * boot_info)
{
	framebuffer = boot_info->framebuffer;
	font = boot_info->font;

	gdt_init();

	idt_init();

	pageframe_init(boot_info->map, boot_info->map_size, boot_info->descriptor_size);

	uint64_t kernel_size = (uint64_t)&_kernelEnd_ - (uint64_t)&_kernelStart_;
	uint64_t kernel_pages = kernel_size / 4096 + 1;
	reserve_pages(&_kernelStart_, kernel_pages);

	paging_init();

	for (uint64_t i = 0;i < total_memory;i += 4096)
		map_memory((void*)i, (void*)i);

	uint64_t fbase = (uint64_t)boot_info->framebuffer->base_address;
	uint64_t fsize = (uint64_t)boot_info->framebuffer->buffer_size;
	reserve_pages((void*)fbase, fsize / 4096 + 1);

	for (uint64_t i = fbase;i < fbase + fsize + 4096;i += 4096)
		map_memory((void*)i, (void*)i);

	asm("mov %0, %%cr3" : : "r"(pml4));

	reserve_pages((void*)0, 0x100);

	heap_init((void*)0x100000000000, 64);

	cpu_init();

	textmode_init();

	disk_init();

	ext2_init();

	shell_init();
	shell_start();

	while (true);
}