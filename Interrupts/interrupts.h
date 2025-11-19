#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <stdint.h>

#define IDT_ENTRIES 256
//types of idt
#define IDT_TASK_GATE 0x5
#define IDT_INTERRUPT_GATE 0xE
#define IDT_TRAP_GATE 0xF
//idt gate descrition
struct idt_gate
{
uint16_t offset_low;
uint16_t selector;
uint8_t zero;
uint8_t type_attr;
uint16_t offset_high;
}__attribute__((packed));
struct idt_ptr
{
uint16_t limit;
uint32_t base;
}__attribute__((packed));
struct interrupt_frame {
        uint32_t gs,fs,es,ds;

    // General-purpose registers from pusha (8 registers)
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    // Interrupt number and error code
    uint32_t int_no, err_code;
    // Automatically pushed by CPU
    uint32_t eip, cs, eflags, user_esp, ss;
};
void init_idt();
void load_idt();
void set_idt_gate(uint8_t num,uint32_t base,uint16_t sel,uint8_t flags);
void irq_install_handler(int irq, void (*handler)(struct interrupt_frame *));
void irq_uninstall(int irq);
//isr handler
void isr_install();
void irq_install();
void load_tss();
// Add this to interrupts.h
void timer_handler(struct interrupt_frame *frame);
#endif