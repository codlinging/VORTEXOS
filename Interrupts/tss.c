// newos/tss.c

#include "tss.h"
#include <stddef.h> // For NULL

// Forward declaration from gdt.c
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

struct tss_entry tss;
extern uint32_t stack_top; // Get stack_top from boot.s

void init_tss() {
    // Get the base address and limit of the TSS structure
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = sizeof(tss);

    // Set the TSS GDT entry (index 5)
    // Access byte 0x89 = Present, DPL 0, Type 9 (32-bit TSS)
    gdt_set_gate(5, base, limit, 0x89, 0x40);

    // Zero out the TSS structure
    // A simple way to do this without memset
    uint8_t *ptr = (uint8_t*)&tss;
    for (uint32_t i = 0; i < sizeof(tss); i++) {
        ptr[i] = 0;
    }

    // Set the kernel stack segment and pointer
    tss.ss0 = 0x10; // Kernel Data Segment selector
    tss.esp0 = (uint32_t)&stack_top;

    // Set iomap_base to prevent I/O port mapping issues
    tss.iomap_base = sizeof(tss);

    // Load the TSS selector (0x28) into the Task Register. THIS IS THE KEY FIX.
    asm volatile("ltr %%ax" : : "a"(0x28));
}

// This function is no longer needed, its logic is in init_tss
// void set_kernel_stack(uint32_t stack) {}