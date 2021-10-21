#include "pic.h"

void pic_remap()
{
	uint8_t a1, a2;

	a1 = inb(PIC1_DATA);
	a1 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 8);
	outb(PIC1_DATA, 4);
	outb(PIC2_DATA, 2);
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}

void pic_set_mask1(uint8_t mask)
{
	outb(PIC1_DATA, mask);
}

void pic_set_mask2(uint8_t mask)
{
	outb(PIC2_DATA, mask);
}

void pic_send_EOI(uint8_t irq)
{
	if (irq >= 8)
		outb(PIC2_COMMAND, PIC_EOI);

	outb(PIC1_COMMAND, PIC_EOI);
}