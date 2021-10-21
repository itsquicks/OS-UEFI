#include "idt.h"

extern IDT _idt[256];

extern uint64_t isr0;
extern uint64_t isr1;
extern uint64_t isr3;
extern uint64_t isr4;
extern uint64_t isr5;
extern uint64_t isr6;
extern uint64_t isr7;
extern uint64_t isr8;
extern uint64_t isr9;
extern uint64_t isr10;
extern uint64_t isr11;
extern uint64_t isr12;
extern uint64_t isr13;
extern uint64_t isr14;
extern uint64_t isr16;
extern uint64_t isr17;
extern uint64_t isr18;
extern uint64_t isr19;
extern uint64_t isr60;

extern "C" void idt_load();

void idt_set(uint64_t isr, uint8_t num)
{
    _idt[num].zero = 0;
    _idt[num].offset_low = (uint16_t)(isr & 0x000000000000ffff);
    _idt[num].offset_mid = (uint16_t)((isr & 0x00000000ffff0000) >> 16);
    _idt[num].offset_high = (uint32_t)((isr & 0xffffffff00000000) >> 32);
    _idt[num].ist = 0;
    _idt[num].selector = 0x08;
    _idt[num].types_attr = 0x8e;
}

void idt_init()
{
    idt_set((uint64_t)&isr0, 0);
    idt_set((uint64_t)&isr1, 1);
    idt_set((uint64_t)&isr3, 3);
    idt_set((uint64_t)&isr4, 4);
    idt_set((uint64_t)&isr5, 5);
    idt_set((uint64_t)&isr6, 6);
    idt_set((uint64_t)&isr7, 7);
    idt_set((uint64_t)&isr8, 8);
    idt_set((uint64_t)&isr9, 9);
    idt_set((uint64_t)&isr10, 10);
    idt_set((uint64_t)&isr11, 11);
    idt_set((uint64_t)&isr12, 12);
    idt_set((uint64_t)&isr13, 13);
    idt_set((uint64_t)&isr14, 14);
    idt_set((uint64_t)&isr16, 16);
    idt_set((uint64_t)&isr17, 17);
    idt_set((uint64_t)&isr18, 18);
    idt_set((uint64_t)&isr19, 19);
    idt_set((uint64_t)&isr60, 60);

    pic_remap();
    pic_set_mask1(0b11111101);
    pic_set_mask2(0b11111111);

    idt_load();
}

void print_error(char* str)
{
    if (cursor_position % textmode_width != 0)
        print_char('\n');

    colorf = RED;
    print_string("Error: ");

    colorf = FRONT;
    print_string(str);
}

extern "C" void isr0_handler()
{
    print_error("Division by zero.");

    pic_send_EOI(0);

    asm("hlt");
}

extern "C" void isr1_handler()
{
    uint8_t scan_code = inb(0x60);
    uint8_t chr = 0;

    if (scan_code < 0x3a)
        chr = scan_code_lookup[scan_code];
    else if (scan_code == 0x56)
        chr = scan_code_lookup[0x2b];

    simple_keyboard_handler(scan_code, chr);

    pic_send_EOI(1);
}

extern "C" void isr3_handler(void)
{
    print_error("Breakpoint - trap.");

    pic_send_EOI(3);

    asm("hlt");
}

extern "C" void isr4_handler(void)
{
    print_error("Overflow.");

    pic_send_EOI(4);

    asm("hlt");
}

extern "C" void isr5_handler(void)
{
    print_error("Bound Range Exceeded.");

    pic_send_EOI(5);

    asm("hlt");
}

extern "C" void isr6_handler(void)
{
    print_error("Invalid Opcode.");

    pic_send_EOI(6);

    asm("hlt");
}

extern "C" void isr7_handler(void)
{
    print_error("Device Not Available (No Math Coprocessor).");

    pic_send_EOI(7);

    asm("hlt");
}

extern "C" void isr8_handler(void)
{
    print_error("Double Fault.");

    pic_send_EOI(8);

    asm("hlt");
}

extern "C" void isr9_handler(void)
{
    print_error("Coprocessor Segment Overrun (reserved).");

    pic_send_EOI(9);

    asm("hlt");
}

extern "C" void isr10_handler(void)
{
    print_error("Invalid TSS.");

    pic_send_EOI(10);

    asm("hlt");
}

extern "C" void isr11_handler(void)
{
    print_error("Segment Not Present.");

    pic_send_EOI(11);

    asm("hlt");
}

extern "C" void isr12_handler(void)
{
    print_error("Stack-Segment Fault.");

    pic_send_EOI(12);

    asm("hlt");
}

extern "C" void isr13_handler(void)
{
    print_error("General Protection Fault.");

    pic_send_EOI(13);

    asm("hlt");
}

extern "C" void isr14_handler(void)
{
    print_error("Page Fault.");

    pic_send_EOI(14);

    asm("hlt");
}

extern "C" void isr16_handler(void)
{
    print_error("x87 FPU Floating-Point Error (Math Fault).");

    pic_send_EOI(16);

    asm("hlt");
}

extern "C" void isr17_handler(void)
{
    print_error("Alignment Check.");

    pic_send_EOI(17);

    asm("hlt");
}

extern "C" void isr18_handler(void)
{
    print_error("Machine Check.");

    pic_send_EOI(18);

    asm("hlt");
}

extern "C" void isr19_handler(void)
{
    print_error("SIMD Floating-Point Exception.");

    pic_send_EOI(19);

    asm("hlt");
}

extern "C" void isr60_handler(void)
{
    return;
}