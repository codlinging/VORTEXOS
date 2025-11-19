ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 2M;
    
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }
    
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
        tss = .;  /* Place TSS in data section */
        . = . + 104; /* TSS size is 104 bytes */
    }
    
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        stack_bottom = .;
        . = . + 2M; /* 2MB stack */
        stack_top = .;
    }
    
    .idt BLOCK(4K) : ALIGN(4K)
    {
        idt = .;
        . = . + 256 * 8;
    }
}