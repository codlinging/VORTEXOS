#include "timer.h"
#include "port.h"
#include "interrupts.h"
#include "pic.h"

#define PIT_FREQUENCY 1193180
#define TIMER_COMMAND 0x43
#define TIMER_DATA 0x40

static volatile uint32_t system_ticks = 0;

void timer_handler(struct interrupt_frame *frame) {
    system_ticks++;
    pic_send_eoi(0); // Timer is IRQ0
}

void init_timer(uint32_t frequency) {
    // Install timer interrupt handler
    irq_install_handler(0, timer_handler);
    
    // Calculate divisor
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    // Send command byte
    outb(TIMER_COMMAND, 0x36);
    
    // Send divisor bytes
    outb(TIMER_DATA, divisor & 0xFF);
    outb(TIMER_DATA, (divisor >> 8) & 0xFF);
    
    // Enable timer IRQ
    pic_clear_mask(0);
}

uint32_t get_system_ticks(void) {
    return system_ticks;
}

void sleep(uint32_t milliseconds) {
    uint32_t target_ticks = system_ticks + milliseconds;
    while (system_ticks < target_ticks) {
        asm volatile("hlt");
    }
}