#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <stdint.h>
#include "rmfs.h"  // Include for MAX_FILENAME_LENGTH

#define NOTEPAD_BUFFER_SIZE 4096
#define MAX_LINE_LENGTH 80

void notepad_loop(const char *filename);
void draw_notepad_icon(int x, int y);
int get_filename_from_user(char *filename_buffer);

#endif