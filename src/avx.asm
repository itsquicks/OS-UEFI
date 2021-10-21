[bits 64]

cpu_enable_sse:

    mov rax, cr0
    and ax, 0xFFFB		; Clear coprocessor emulation CR0.EM
    or ax, 0x2			; Set coprocessor monitoring  CR0.MP
    or eax,0x40000000
    mov cr0, rax

    mov rax, cr4
    or ax, 3 << 9		; Set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, rax
    ret

GLOBAL cpu_enable_sse

cpu_enable_osxsave:

    mov rax, cr4
    or eax, 1 << 18     ; Set CR4.OSXSAVE
    mov cr4, rax
    ret

GLOBAL cpu_enable_osxsave

cpu_enable_avx:

    xor rdx,rdx
    xor rcx,rcx
    xor rax,rax
    xgetbv
    or rax, 0b0111
    xsetbv
    ret

GLOBAL cpu_enable_avx

cpu_get_cr0:

    mov rax,cr0
    ret

GLOBAL cpu_get_cr0

cpu_get_xcr0:

    xor rcx,rcx
    xor rax,rax
    xgetbv
    ret

GLOBAL cpu_get_xcr0