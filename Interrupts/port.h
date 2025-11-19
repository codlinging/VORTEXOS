#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

// Output byte to port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "dN"(port));
}

// Input byte from port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}
static inline void io_wait(void)
{
outb(0x80,0);
}

#endif