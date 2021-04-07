#include "Typedefs.h"
#include "GDT.h"
#include "IDT.h"
#include "Graphics.h"
#include "TextMode.h"
#include "Drivers/PCI.h"
#include "Drivers/CPU.h"
#include "Memory/PageFrameAllocator.h"
#include "Memory/Paging.h"
#include "CommandLine.h"
#include "Filesystem.h"

struct BootInfo
{
	Framebuffer* framebuffer;
	PSF2_FONT* font;
	void* map;
	uint64 mapSize;
	uint64 descriptorSize;
};

extern uint64 _kernelStart_;
extern uint64 _kernelEnd_;

extern "C" void _start(BootInfo * bootInfo)
{
	framebuffer = bootInfo->framebuffer;
	font = bootInfo->font;

	GDTDescriptor gdtDescriptor;
	gdtDescriptor.size = sizeof(GDT) - 1;
	gdtDescriptor.offset = (uint64)&gdt;
	LoadGDT(&gdtDescriptor);

	InitializeIDT();

	InitializePageFrameAllocator(bootInfo->map, bootInfo->mapSize, bootInfo->descriptorSize);

	uint64 kernelSize = (uint64)&_kernelEnd_ - (uint64)&_kernelStart_;
	uint64 kernelPages = kernelSize / 4096 + 1;
	ReservePages(&_kernelStart_, kernelPages);

	InitializePaging();

	for (uint64 i = 0;i < totalMemory;i += 4096)
		MapMemory((void*)i, (void*)i);

	uint64 fbase = (uint64)bootInfo->framebuffer->BaseAddress;
	uint64 fsize = (uint64)bootInfo->framebuffer->BufferSize;
	ReservePages((void*)fbase, fsize / 4096 + 1);

	for (uint64 i = fbase;i < fbase + fsize + 4096;i += 4096)
		MapMemory((void*)i, (void*)i);

	asm("mov %0, %%cr3" : : "r"(PML4));

	ReservePages((void*)0x0, 0x100);

	InitializeTextMode();

	InitializeFilesystem();

	InitializeProcessManager();

	InitializeCommandLine();
	StartCommandLine();

	while (1);
}