; gdt_asm.s
section .text
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]  ; Get the GDT pointer
    lgdt [eax]          ; Load the GDT

    mov ax, 0x10        ; 0x10 is the offset of our kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Jump to the new code segment to flush the CS register
    jmp 0x08:.flush     ; 0x08 is the offset of our kernel code segment
.flush:
    ret