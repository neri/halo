;
;   HALO: HAribote-os LOader for EFI
;
;   Copyright (c) 2016 Nerry, All rights reserved.
;
;   Permission is hereby granted, free of charge, to any person obtaining a copy
;   of this software and associated documentation files (the "Software"), to deal
;   in the Software without restriction, including without limitation the rights
;   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;   copies of the Software, and to permit persons to whom the Software is
;   furnished to do so, subject to the following conditions:
;
;   The above copyright notice and this permission notice shall be included in all
;   copies or substantial portions of the Software.
;
;   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;   SOFTWARE.
;

%define LOADER_CS   0x18
%define LOADER_DS   0x08

[bits 64]
[section .text]


; jmp_buf:
;   RAX EBX ECX EDX RSI EDI RBP RSP R8_ R9_ R10 R11 R12 R13 R14 R15 RIP
;       8                   48   0                  16  24  32  40  56

; int _setjmp(jmp_buf env);
global _setjmp
_setjmp:
    push rbp
    mov rbp, rsp

    lea rax, [rsp+16]
    mov [rdi   ], rax

    mov [rdi+ 8], rbx
    mov [rdi+16], r12
    mov [rdi+24], r13
    mov [rdi+32], r14
    mov [rdi+40], r15

    mov rax, [rbp]
    mov rdx, [rbp+ 8]
    mov [rdi+48], rax
    mov [rdi+56], rdx

    xor eax, eax
    leave
    ret


; void _longjmp(jmp_buf env, int retval);
global _longjmp
_longjmp:
    mov rsp, [rdi   ]
    mov rbx, [rdi+ 8]
    mov r12, [rdi+16]
    mov r13, [rdi+24]
    mov r14, [rdi+32]
    mov r15, [rdi+40]
    mov rbp, [rdi+48]
    mov rdx, [rdi+56]

    mov eax, esi
    or eax, eax
    jnz .nozero
    inc eax
.nozero:
    jmp rdx


global strncmp
; int strncmp(const char* s1, const char* s2, size_t n)
strncmp:
.loop:
    mov al, [rdi]
    or al, al
    jz .last
    sub al, [rsi]
    jnz .end
    inc rsi
    inc rdi
    loop .loop
    jmp short .end
.last:
    sub al, [rsi]
.end:
    movsx rax, al
    ret


; void start_kernel(i386_context_t* regs)
global start_kernel
start_kernel:

    ; mask IRQ
    mov al, 0xFF
    out 0x21, al
    pause
    out 0xA1, al

    ; block IRQ
    cli

    ; load GDTR
    lea rbx, [rel __GDT]
    mov [rbx+2], rbx
    lgdt [rbx]

    ; copy context
    mov rsi, rdi
    mov ebp, [rsi+0x48]
    sub ebp, byte 0x48
    mov edi, ebp
    mov ecx, 0x11
    rep movsd

    ; jump to 32bit segment
    push byte LOADER_DS
    push rbp
    pushfq
    push byte LOADER_CS
    lea rax, [rel _now_in_32bit]
    push rax
    iretq

[bits 32]

_now_in_32bit:

    ; setup segments
    mov eax, LOADER_DS
    mov ds, eax
    mov es, eax

    ; disable paging (leave long-mode)
    mov eax, cr0
    btr eax, 31
    mov cr0, eax

_now_in_prot:

    ; disable misc flags
    mov eax, cr4
    and eax, 0xFFFFFD5F ; OSFXSR/PGE/PAE
    mov cr4, eax

    ; disable long mode
    mov ecx ,0xC0000080
    rdmsr
    btr eax, 8 ; LME
    btr eax, 11 ; NXE
    wrmsr

    ; setup registers
    add esp, byte 4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, byte 8

    ; goto kernel
    retf


[section .data]
__GDT:
    dw (__end_GDT-__GDT-1), 0, 0, 0 ; NULL
    dw 0xFFFF,0x0000,0x9200,0x00CF  ; 08 32bit KERNEL DATA FLAT
    dw 0xFFFF,0x0000,0x9A28,0x0047  ; 10 32bit KERNEL TEXT haribote
    dw 0xFFFF,0x0000,0x9A00,0x00CF  ; 18 32bit KERNEL TEXT FLAT
__end_GDT:
