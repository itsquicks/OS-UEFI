[extern _idt]
idt_descriptor:
dw 4095
dq _idt

%macro PUSHALL 0
push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
%endmacro

%macro POPALL 0
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
%endmacro

[extern isr0_handler]
[extern isr1_handler]
[extern isr3_handler]
[extern isr4_handler]
[extern isr5_handler]
[extern isr6_handler]
[extern isr7_handler]
[extern isr8_handler]
[extern isr9_handler]
[extern isr10_handler]
[extern isr11_handler]
[extern isr12_handler]
[extern isr13_handler]
[extern isr14_handler]
[extern isr16_handler]
[extern isr17_handler]
[extern isr18_handler]
[extern isr19_handler]
[extern isr60_handler]

isr0:
PUSHALL
call isr0_handler
POPALL
iretq
GLOBAL isr0

isr1:
PUSHALL
call isr1_handler
POPALL
iretq
GLOBAL isr1

isr3:
PUSHALL
call isr3_handler
POPALL
iretq
GLOBAL isr3

isr4:
PUSHALL
call isr4_handler
POPALL
iretq
GLOBAL isr4

isr5:
PUSHALL
call isr5_handler
POPALL
iretq
GLOBAL isr5

isr6:
PUSHALL
call isr6_handler
POPALL
iretq
GLOBAL isr6

isr7:
PUSHALL
call isr7_handler
POPALL
iretq
GLOBAL isr7

isr8:
PUSHALL
call isr8_handler
POPALL
iretq
GLOBAL isr8

isr9:
PUSHALL
call isr9_handler
POPALL
iretq
GLOBAL isr9

isr10:
PUSHALL
call isr10_handler
POPALL
iretq
GLOBAL isr10

isr11:
PUSHALL
call isr11_handler
POPALL
iretq
GLOBAL isr11

isr12:
PUSHALL
call isr12_handler
POPALL
iretq
GLOBAL isr12

isr13:
PUSHALL
call isr13_handler
POPALL
iretq
GLOBAL isr13

isr14:
PUSHALL
call isr14_handler
POPALL
iretq
GLOBAL isr14

isr16:
PUSHALL
call isr16_handler
POPALL
iretq
GLOBAL isr16

isr17:
PUSHALL
call isr17_handler
POPALL
iretq
GLOBAL isr17

isr18:
PUSHALL
call isr18_handler
POPALL
iretq
GLOBAL isr18

isr19:
PUSHALL
call isr19_handler
POPALL
iretq
GLOBAL isr19

isr60:
PUSHALL
call isr60_handler
POPALL
iretq
GLOBAL isr60

idt_load:
lidt [idt_descriptor]
sti
ret
GLOBAL idt_load
