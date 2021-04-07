#include "IO.h"

void outb(uint16 port, uint8 val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8 inb(uint16 port)
{
	uint8 ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void outw(uint16 port, uint16 val)
{
	asm volatile ("outw %0, %1" : : "a"(val), "d"(port));
}

uint16 inw(uint16 port)
{
	uint16 ret;
	asm volatile ("inw %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

void outl(uint16 port, uint32 val)
{
	asm volatile ("outl %0, %1" : : "a"(val), "d"(port));
}

uint32 inl(uint16 port)
{
	uint32 ret;
	asm volatile ("inl %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}