#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "interrupts.h"

void init_keyboard();
void keyboard_handler(struct interrupt_frame *frame);
char keyboard_getchar();
int keyboard_has_input();
const char* keyboard_get_buffer();
void keyboard_clear_buffer();
int keyboard_ctrl_pressed(void);
int keyboard_shift_pressed(void);
int keyboard_alt_pressed(void);

#endif