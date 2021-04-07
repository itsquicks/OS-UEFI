#pragma once
#include "../Typedefs.h"
#include "../IO.h"
#include "../Memory/PageFrameAllocator.h"

void CPUID(uint32 code, uint32& eax, uint32& ebx, uint32& ecx, uint32& edx);
char* CPU_GetName();
uint64 CPU_GetSpeed();
uint64 RDTSC();