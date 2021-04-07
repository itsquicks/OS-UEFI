#include "CPU.h"

void CPUID(uint32 code, uint32& eax, uint32& ebx, uint32& ecx, uint32& edx)
{
    asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(code));
}

char* Get_CPU_Name()
{
    uint32* cpu_name = (uint32*)RequestPage();

    CPUID(0x80000002, *(cpu_name + 0x0), *(cpu_name + 0x1), *(cpu_name + 0x2), *(cpu_name + 0x3));
    CPUID(0x80000003, *(cpu_name + 0x4), *(cpu_name + 0x5), *(cpu_name + 0x6), *(cpu_name + 0x7));
    CPUID(0x80000004, *(cpu_name + 0x8), *(cpu_name + 0x9), *(cpu_name + 0xa), *(cpu_name + 0xb));

    FreePage(cpu_name);

    return (char*)cpu_name;
}

uint64 Get_CPU_Speed()
{
    asm volatile("cli");

    outb(0x43, 0x34);
    outb(0x40, 0);
    outb(0x40, 0);

    uint64 cpu_speed;

    uint64 stsc = RDTSC();
    for (uint16 i = 0x1000; i > 0; i--);
    uint64  etsc = RDTSC();

    outb(0x43, 0x04);
    uint8 lo = inb(0x40);
    uint8 hi = inb(0x40);

    asm volatile("sti");

    uint64 ticks = (0x10000 - (hi * 256 + lo));
    cpu_speed = (etsc - stsc) * 1193180 / ticks;
    cpu_speed /= 1000 * 1000;

    return cpu_speed;
}

uint64 RDTSC()
{
    uint64 ret = 0;
    uint32 eax;
    uint32 edx;

    asm volatile("rdtsc" : "=a"(eax), "=d"(edx) : );

    ret += edx * 256;
    ret += eax;
    return ret;
}
