#pragma once
#include "../defs.h"
#include "../io.h"
#include "../heap.h"
#include "../utils.h"

extern uint32_t* cpu_name;
extern uint32_t* cpu_flags;

void cpu_init();
void cpu_cpuid(uint32_t fn, uint32_t sfn, uint32_t& eax, uint32_t& ebx, uint32_t& ecx, uint32_t& edx);
uint64_t cpu_rdtsc();
uint64_t cpu_get_speed();
