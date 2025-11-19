#include "pic.h"
#include "port.h"
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01



void pic_remap()
{
    uint8_t a1=inb(PIC1_DATA);
    uint8_t a2=inb(PIC2_DATA);

    outb(PIC1_COMMAND,ICW1_INIT|ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND,ICW1_INIT|ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA,0x20);//master pic
     io_wait();   
    outb(PIC2_DATA,0x28);//slave pic
     io_wait();   
    outb(PIC1_DATA,4);
    io_wait();
    
    outb(PIC2_DATA,2);
        io_wait();

    //set 8086
    outb(PIC1_DATA,ICW4_8086);
        io_wait();

    outb(PIC2_DATA,ICW4_8086);
        io_wait();

    //restore mask
    outb(PIC1_DATA,a1);
    outb(PIC2_DATA,a2);

}

void pic_send_eoi(uint8_t irq)
{
if(irq>=8)
{
outb(PIC2_COMMAND,0x20);
}
outb(PIC1_COMMAND,0x20);
}
void pic_set_mask(uint8_t irq)
{
uint16_t port;
uint8_t value;
if(irq<8)
{
    port=PIC1_DATA;
}
else
{
    port=PIC2_DATA;
    irq-=8;
}
value=inb(port)|(1<<irq);
outb(port,value);
}
void pic_clear_mask(uint8_t irq)
{
uint16_t port;
uint8_t value;
if(irq<8)
{
    port=PIC1_DATA;
}
else{
    port=PIC2_DATA;
    irq-=8;
}
value=inb(port) & ~(1<<irq);
outb(port,value);
}