; boot.s
section .multiboot
align 4
multiboot_header_start:
    dd 0x1BADB002
    dd 0x00000006 ; Use flags for framebuffer
    dd -(0x1BADB002 + 0x00000006)
    ; Graphics fields for multiboot
    dd multiboot_header_start
    dd 0
    dd 0
    dd 0
    dd start
    dd 0 ; mode_type
    dd 1024 ; width
    dd 768  ; height
    dd 32   ; depth
multiboot_header_end:

section .text
extern kernel_main
global start

start:
    ; CPU is in 32-bit protected mode.
    ; Multiboot info is in ebx.
    ; Stack is not set yet.
    
    cli ; Disable interrupts until IDT is ready
    
    mov esp, stack_top
    
    push ebx ; multiboot info struct
    push eax ; magic number
    
    call kernel_main

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 2 * 1024 * 1024 ; 2MB stack
stack_top:

global stack_top