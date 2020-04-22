;
;   HALO: HAribote-os LOader for EFI
;
;   Copyright (c) 2016,2018 Nerry, All rights reserved.
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

[bits 32]
[section .text]


; int _setjmp3(jmp_buf env, int count, ... /* OPTIONAL PARAMETERS */);
global __setjmp3
__setjmp3:
    ;  TODO: NOT IMPLEMENTED
    int 3


; void _longjmp(jmp_buf env, int retval);
global __longjmp
__longjmp:
    ;  TODO: NOT IMPLEMENTED
    int 3


; int strncmp(const char* s1, const char* s2, size_t n)
global _strncmp
_strncmp:
    push ebp
    mov ebp, esp
    push ebx
    mov ebx, [ebp+8]
    mov edx, [ebp+12]
    mov ecx, [ebp+16]

.loop:
    mov al, [ebx]
    or al, al
    jz .last
    sub al, [edx]
    jnz .end
    inc ebx
    inc edx
    loop .loop
    jmp short .end
.last:
    sub al, [edx]
.end:
    movsx eax, al

    pop ebx
    pop ebp
    ret


; void* memset(void* p, int val, size_t n)
global _memset
_memset:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    mov al, [ebp+12]
    mov ecx, [ebp+16]

    mov edi, edx
    rep stosb

    mov eax, edx
    pop ebp
    ret


; void start_kernel(i386_context_t* regs)
global _start_kernel
_start_kernel:

    ; mask IRQ
    mov al, 0xFF
    out 0x21, al
    out 0xA1, al

    ; block IRQ
    cli

    ; copy context
    mov esi, [esp+4]
    mov ebp, [esi+0x48]
    sub ebp, byte 0x48
    mov edi, ebp
    mov ecx, 0x11
    rep movsd

    ; load GDTR
    mov ebx, __GDT
    mov [ebx+2], ebx
    lgdt [ebx]

    ; use new segment
    mov eax, LOADER_DS
    mov ds, eax
    mov es, eax
    mov ss, eax
    mov esp, ebp
    push BYTE LOADER_CS
    call _retf

    ; disable paging
    mov eax, cr0
    btr eax, 31
    mov cr0, eax

    ; disable misc flags
    mov eax, cr4
    and eax, 0xFFFFFD5F ; OSFXSR/PGE/PAE
    mov cr4, eax

    ; setup context registers
    add esp, byte 4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, byte 8

    ; goto kernel
_retf:
    retf


[section .data]
__GDT:
    dw (__end_GDT-__GDT-1), 0, 0, 0 ; NULL
    dw 0xFFFF,0x0000,0x9200,0x00CF  ; 08 32bit KERNEL DATA FLAT
    dw 0xFFFF,0x0000,0x9A28,0x0047  ; 10 32bit KERNEL TEXT haribote
    dw 0xFFFF,0x0000,0x9A00,0x00CF  ; 18 32bit KERNEL TEXT FLAT
__end_GDT:
