#include "cpu.h"

uint32_t* cpu_name;
uint32_t* cpu_flags;

extern "C" void cpu_enable_sse();
extern "C" void cpu_enable_osxsave();
extern "C" void cpu_enable_avx();

extern "C"  uint32_t cpu_get_cr0();
extern "C"  uint64_t cpu_get_xcr0();

void cpu_cpuid(uint32_t fn, uint32_t sfn, uint32_t& eax, uint32_t& ebx, uint32_t& ecx, uint32_t& edx)
{
    asm volatile
        ("cpuid" : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (fn), "c" (sfn));
}

uint64_t cpu_rdtsc()
{
    uint64_t ret = 0;
    uint32_t eax;
    uint32_t edx;

    asm volatile("rdtsc" : "=a"(eax), "=d"(edx) : );

    ret += edx * 256;
    ret += eax;
    return ret;
}

void cpu_init()
{
    cpu_name = (uint32_t*)malloc(48);
    cpu_flags = (uint32_t*)malloc(16);

    cpu_cpuid(0x80000002, 0, *(cpu_name + 0x0), *(cpu_name + 0x1), *(cpu_name + 0x2), *(cpu_name + 0x3));
    cpu_cpuid(0x80000003, 0, *(cpu_name + 0x4), *(cpu_name + 0x5), *(cpu_name + 0x6), *(cpu_name + 0x7));
    cpu_cpuid(0x80000004, 0, *(cpu_name + 0x8), *(cpu_name + 0x9), *(cpu_name + 0xa), *(cpu_name + 0xb));

    cpu_cpuid(0x00000007, 0, *(cpu_flags + 0x0), *(cpu_flags + 0x1), *(cpu_flags + 0x2), *(cpu_flags + 0x3));

    //cpu_enable_sse();
    //cpu_enable_osxsave();
    //cpu_enable_avx();
}

uint64_t cpu_get_speed()
{
    asm volatile("cli");

    outb(0x43, 0x34);
    outb(0x40, 0);
    outb(0x40, 0);

    uint64_t cpu_speed;

    uint64_t stsc = cpu_rdtsc();
    for (uint16_t i = 0x1000; i > 0; i--);
    uint64_t  etsc = cpu_rdtsc();

    outb(0x43, 0x04);
    uint8_t lo = inb(0x40);
    uint8_t hi = inb(0x40);

    asm volatile("sti");

    uint64_t ticks = (0x10000 - (hi * 256 + lo));
    cpu_speed = (etsc - stsc) * 1193180 / ticks;
    cpu_speed /= 1000 * 1000;

    return cpu_speed;
}