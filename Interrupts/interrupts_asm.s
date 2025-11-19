section .text

; Macro for ISR without error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

; Macro for ISR with error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

; Macro for IRQ
%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

; Define ISRs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; Define IRQs (remapped to 0x20-0x2F)
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Common ISR stub
extern isr_handler
isr_common_stub:
    pusha        ; Push general-purpose registers FIRST (matches struct)
    push ds      ; Then push segment registers
    push es
    push fs
    push gs

    mov ax, 0x10 ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp     ; Push pointer to the frame for the C handler
    call isr_handler
    add esp, 4   ; Clean up stack after call

    pop gs       ; Pop segment registers first (reverse order)
    pop fs
    pop es
    pop ds
    popa         ; Then pop general-purpose registers
    
    add esp, 8   ; Clean up error code and interrupt number
    iret

; Common IRQ stub
extern irq_handler
irq_common_stub:
    pusha        ; Push general-purpose registers FIRST (matches struct)
    push ds      ; Then push segment registers
    push es
    push fs
    push gs

    mov ax, 0x10 ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp     ; Push pointer to the frame for the C handler
    call irq_handler
    add esp, 4   ; Clean up stack after call

    pop gs       ; Pop segment registers first (reverse order)
    pop fs
    pop es
    pop ds
    popa         ; Then pop general-purpose registers
    
    add esp, 8   ; Clean up error code and interrupt number
    iret

; Load IDT
global load_idt
load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret