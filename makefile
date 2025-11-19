# Makefile
CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra
AS = nasm
ASFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o Drivers/vesa.o Interrupts/interrupts.o Interrupts/pic.o Interrupts/interrupts_asm.o Interrupts/tss.o Interrupts/gdt.o Interrupts/gdt_asm.o Drivers/keyboard.o application/shell.o Drivers/string.o application/snakegame.o Interrupts/timer.o Drivers/rmfs.o application/notepad.o
ISO = vortexos.iso

# Build everything
all: $(ISO)

# Rule to build the kernel
kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Rules for object files
boot.o: boot.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.o: kernel.c vesa.h
	$(CC) $(CFLAGS) -c $< -o $@

vesa.o: vesa.c vesa.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the bootable ISO
$(ISO): kernel.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO) isodir

# Clean up all generated files
clean:
	rm -f *.o *.bin $(ISO)
	rm -rf isodir

# Run QEMU with the ISO
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -vga std -display sdl