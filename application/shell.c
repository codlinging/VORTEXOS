#include "../Drivers/rmfs.h"
#include "notepad.h"
#include "../Drivers/keyboard.h"
#include "../Drivers/string.h"
#include "../Drivers/vesa.h" 
#include "shell.h"
#include "../Drivers/colours.h"
#define LINE_HEIGHT (8*2+4)// Font height * scale + padding
#define PROMPT_OFFSET 20
#define TEXT_OFFSET 180
static char cmd_buffer[MAX_COMMAND_LENGTH];
static int cursor_pos = 0;
static char cmd_his[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int hist_cnt = 0;
static int hist_ind = 0;
static int current_line = 2;  // Track current line position

void init_shell(void) {
    cmd_buffer[0] = 0;
    cursor_pos = 0;
    hist_cnt = 0;
    hist_ind = 0;
    current_line = 1;  // Start from line 1 (line 0 is for prompt)
}
 
void print_prompt(void) {
    draw_string_scaled(PROMPT_OFFSET, (current_line) * LINE_HEIGHT, "VortexOS-> ", COLOR_WHITE, 2);
    // Display current command buffer
    draw_string_scaled(PROMPT_OFFSET+ 176, current_line * LINE_HEIGHT, cmd_buffer, COLOR_GRAY,2);
}

void execute_command(char *input) {
    char *args[MAX_ARGS];
    int arg_count = 0;
    char input_copy[MAX_COMMAND_LENGTH];
    mystrcpy(input_copy, input);
    char *token = input_copy;
    
    for(int i = 0; i < MAX_ARGS - 1; i++) {
        // Skip front spaces
        while(*token == ' ') token++;
        if(*token == '\0') break;
        args[arg_count++] = token;
        // Find end of token
        while(*token && *token != ' ') token++;
        if(*token == ' ') {
            *token++ = '\0';
        }
    }
    args[arg_count] = NULL;
    if(arg_count == 0) return;
    
    // Add to history
    if(hist_cnt < MAX_HISTORY && mystrlen(input) > 0) {
        mystrcpy(cmd_his[hist_cnt], input);
        hist_cnt++;
    }
    
    if(mystrcmp(args[0], "help") == 0) {
        draw_string_scaled(10, (current_line + 1) * 20, "Available commands:", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 2) * 20, "  help    - Show this help", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 3) * 20, "  clear   - Clear screen", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 4) * 20, "  echo    - Print text", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 5) * 20, "  info    - System information", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 6) * 20, "  reboot  - Reboot system", COLOR_DARK_GREEN,2);
        current_line += 7;
    }
    else if(mystrcmp(args[0], "clear") == 0) {
        fill_screen(COLOR_BLACK);
        current_line = 1;
    }
    else if(mystrcmp(args[0], "echo") == 0) {
        char output[MAX_COMMAND_LENGTH] = "";
        for(int i = 1; i < arg_count; i++) {  // Start from 1 to skip "echo"
            mystrcat(output, args[i]);
            if(i < arg_count - 1) {
                mystrcat(output, " ");
            }
        }
        draw_string_scaled(10, (current_line + 1) * 20, output, COLOR_BLUE,2);
        current_line += 2;
    }
    else if(mystrcmp(args[0], "info") == 0) {
        draw_string_scaled(10, (current_line + 1) * 20, "VortexOS v0.1 - Simple x86 Operating System", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 2) * 20, "Framebuffer: 1024x768x32", COLOR_DARK_GREEN,2);
        draw_string_scaled(10, (current_line + 3) * 20, "Shell: Basic command interpreter", COLOR_DARK_GREEN,2);
        current_line += 4;
    }

// Add to execute_command function, after the existing commands:
else if(mystrcmp(args[0], "edit") == 0) {
    if (arg_count < 2) {
        draw_string_scaled(10, (current_line + 1) * 20, "Usage: edit <filename>", COLOR_DARK_GREEN, 2);
        current_line += 2;
    } else {
        // Launch notepad with the specified filename
        notepad_loop(args[1]);
        // Redraw shell after returning from notepad
        fill_screen(COLOR_BLACK);
        current_line = 1;
        print_prompt();
        return; // Skip the normal command processing
    }
}
else if(mystrcmp(args[0], "ls") == 0 || mystrcmp(args[0], "dir") == 0) {
    char file_list[1024];
    list_files(file_list, sizeof(file_list));
    
    char *line = file_list;
    char *next_line;
    while (*line) {
        next_line = line;
        while (*next_line && *next_line != '\n') next_line++;
        if (*next_line == '\n') {
            *next_line = '\0';
            next_line++;
        }
        
        draw_string_scaled(10, (current_line + 1) * 20, line, COLOR_DARK_GREEN, 2);
        current_line++;
        line = next_line;
    }
    if (current_line == 1) {
        current_line += 2;
    }
}
else if(mystrcmp(args[0], "cat") == 0) {
    if (arg_count < 2) {
        draw_string_scaled(10, (current_line + 1) * 20, "Usage: cat <filename>", COLOR_DARK_GREEN, 2);
        current_line += 2;
    } else {
        if (file_exists(args[1])) {
            char file_content[NOTEPAD_BUFFER_SIZE];
            int bytes_read = read_files(args[1], file_content, sizeof(file_content) - 1);
            if (bytes_read > 0) {
                file_content[bytes_read] = '\0';
                draw_string_scaled(10, (current_line + 1) * 20, file_content, COLOR_BLUE, 2);
                current_line += 2;
            }
        } else {
            char error_msg[64];
            mystrcpy(error_msg, "File not found: ");
            mystrcat(error_msg, args[1]);
            draw_string_scaled(10, (current_line + 1) * 20, error_msg, COLOR_DARK_GREEN, 2);
            current_line += 2;
        }
    }
}
else if(mystrcmp(args[0], "rm") == 0 || mystrcmp(args[0], "del") == 0) {
    if (arg_count < 2) {
        draw_string_scaled(10, (current_line + 1) * 20, "Usage: rm <filename>", COLOR_DARK_GREEN, 2);
        current_line += 2;
    } else {
        if (delete_files(args[1]) == 0) {
            char success_msg[64];
            mystrcpy(success_msg, "Deleted: ");
            mystrcat(success_msg, args[1]);
            draw_string_scaled(10, (current_line + 1) * 20, success_msg, COLOR_DARK_GREEN, 2);
        } else {
            char error_msg[64];
            mystrcpy(error_msg, "File not found: ");
            mystrcat(error_msg, args[1]);
            draw_string_scaled(10, (current_line + 1) * 20, error_msg, COLOR_DARK_GREEN, 2);
        }
        current_line += 2;
    }
}
    else {
        char error_msg[64];
        mystrcpy(error_msg, "Unknown command: ");
        mystrcat(error_msg, args[0]);
        draw_string_scaled(10, (current_line + 1) * 20, error_msg, COLOR_DARK_GREEN,2);
        current_line += 2;
    }
    
    // Check if we're running out of screen space
    if(current_line * 20 > fb.height - 40) {
        // Simple scroll - just reset to top
        fill_screen(0x00FFFFFF);
        current_line = 1;
    }
}

void shell_loop(void) {
    init_shell();
    fill_screen(COLOR_BLACK); 
    print_prompt();
    // In shell_loop() function, replace the input handling section with this:

while (1) {
    if (keyboard_has_input()) {
        char c = keyboard_getchar();
        
        
        // Handle Enter key
        if (c == '\n') {
            // Execute command
            if (cursor_pos > 0) {
                // Display the executed command on a new line
               // draw_string_scaled(30, (current_line + 1) * 20, cmd_buffer, 0xFF000000,2);
                execute_command(cmd_buffer);
                
                // Clear command buffer and reset cursor
                cmd_buffer[0] = '\0';
                cursor_pos = 0;
                
                // Move to next line for new prompt
                current_line++;
                if(current_line * 20 > fb.height - 40) {
                    fill_screen(0x00FFFFFF);
                    current_line = 1;
                }
            }
            print_prompt();
        }
        // Handle backspace - try all possible backspace codes
        else if (c == '\b' || c == 127 || c == 0x08 || c == 0x7F) {
            //fill_rect(400, 420, 300, 20, COLOR_BLACK);
            //draw_string(400, 420, "Backspace detected!", 0xFFFF0000);
            
            if (cursor_pos > 0) {
                cursor_pos--;
                cmd_buffer[cursor_pos] = '\0';
                // Redraw command line to show updated text
                fill_rect(100 + 80, current_line * 20, 200, 20, COLOR_BLACK);
                draw_string_scaled(20 + 160, current_line * 20, cmd_buffer, COLOR_BLACK,2);
            }
        }
        // Handle escape (return to dashboard)
        else if (c == 0x1B) { // ESC key
            return; // Exit shell and return to dashboard
        }
        // Normal character input
        else if (c >= 32 && c <= 126 && cursor_pos < MAX_COMMAND_LENGTH - 1) {
            cmd_buffer[cursor_pos] = c;
            cursor_pos++;
            cmd_buffer[cursor_pos] = '\0';
            // Display current command
           // fill_rect(10 + 80, current_line * 20, 200, 20, 0x00FFFFFF);
            draw_string_scaled(20 + 160, current_line * 20, cmd_buffer, COLOR_GREEN,2);
        }
    }
}}
