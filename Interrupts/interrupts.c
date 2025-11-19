#include "interrupts.h"
#include "vesa.h"
#include "tss.h"
#include "pic.h"
#include "port.h"
#include "keyboard.h"

extern void load_idt(struct idt_ptr *ptr);
extern uint32_t stack_top;

struct idt_gate idt[IDT_ENTRIES];
struct idt_ptr idtp;

// ISR handlers declared in assembly
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// IRQ handlers declared in assembly
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void (*irq_handlers[16])(struct interrupt_frame *) = {0};
//to enable you to clear IRQ masks from other drivers
void pic_clear_irq(uint8_t irq) {
    pic_clear_mask(irq);
}
void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].type_attr = flags | 0x80; // Present bit + DPL 0
}

void init_idt() {
    //init_tss();
    //set_kernel_stack((uint32_t)&stack_top);

    idtp.limit = (sizeof(struct idt_gate) * IDT_ENTRIES) - 1;
    idtp.base = (uint32_t)&idt;

    // Array of ISR functions
    void (*isrs[])() = {
        isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9,
        isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19,
        isr20, isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29,
        isr30, isr31
    };

    // Set IDT gates for ISRs
    for (int i = 0; i < 32; i++) {
        set_idt_gate(i, (uint32_t)isrs[i], 0x08, IDT_INTERRUPT_GATE);
    }
    
    // Array of IRQ functions
    void (*irqs[])() = {
        irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9,
        irq10, irq11, irq12, irq13, irq14, irq15
    };

    // Set IDT gates for IRQs
    for (int i = 0; i < 16; i++) {
        set_idt_gate(32 + i, (uint32_t)irqs[i], 0x08, IDT_INTERRUPT_GATE);
    }
    
    pic_remap();

    //load_tss();
    load_idt(&idtp);
}

void load_tss() {
    asm volatile ("ltr %%ax" : : "a" (0x2B));  // Load TSS selector
}

void isr_handler(struct interrupt_frame *frame) {
    draw_string(10, 10, "Exception Occurred", 0xFFFFFFFF);
    while(1);
}

void irq_handler(struct interrupt_frame *frame) {
    // Send EOI to PIC
    if (frame->int_no >= 40) {
        outb(0xA0, 0x20); // Send EOI to slave
    }
    outb(0x20, 0x20); // Send EOI to master

    if (frame->int_no >= 32 && frame->int_no <= 47) {
        uint8_t irq = frame->int_no - 32;
        if (irq_handlers[irq]) {
            irq_handlers[irq](frame);
        }
    }
}

void irq_install_handler(int irq, void (*handler)(struct interrupt_frame *)) {
    irq_handlers[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_handlers[irq] = 0;
}