
// #include "notepad.h"
// #include "vesa.h"
// #include "keyboard.h"
// #include "rmfs.h"
// #include "string.h"
// #include "colours.h"

// static char notepad_buffer[NOTEPAD_BUFFER_SIZE];
// static int cursor_pos = 0;
// static int scroll_offset = 0;
// static char current_filename[MAX_FILENAME_LENGTH] = "notes.txt";

// void notepad_draw_interface(const char *filename) {
//     fill_screen(COLOR_BLACK);
    
//     // Draw header
//     fill_rect(0, 0, fb.width, 30, COLOR_DARK_BLUE);
//     draw_string_scaled(10, 5, "VortexOS Notepad", COLOR_WHITE, 2);
    
//     // Draw filename
//     char title[100];
//     mystrcpy(title, "File: ");
//     mystrcat(title, filename);
//     draw_string_scaled(fb.width - 300, 5, title, COLOR_WHITE, 2);
    
//     // Draw instructions
//     draw_string_scaled(10, fb.height - 40, "Press ESC to exit, Ctrl+S to save", COLOR_GRAY, 2);
//     draw_string_scaled(10, fb.height - 20, "Use arrow keys to navigate, Backspace to delete", COLOR_GRAY, 2);
    
//     // Draw text area background
//     fill_rect(10, 40, fb.width - 20, fb.height - 80, COLOR_WHITE);
// }

// void notepad_draw_text(void) {
//     // Clear text area
//     fill_rect(20, 50, fb.width - 40, fb.height - 100, COLOR_WHITE);
    
//     // Draw text with line wrapping
//     int x = 20;
//     int y = 50;
//     int line_length = 0;
//     char line[MAX_LINE_LENGTH + 1];
//     int line_pos = 0;
    
//     for (int i = 0; i < cursor_pos && i < NOTEPAD_BUFFER_SIZE - 1; i++) {
//         char c = notepad_buffer[i];
        
//         if (c == '\n' || line_length >= MAX_LINE_LENGTH) {
//             line[line_pos] = '\0';
//             draw_string_scaled(x, y, line, COLOR_BLACK, 2);
            
//             y += 24; // Line height
//             x = 20;
//             line_length = 0;
//             line_pos = 0;
            
//             if (c == '\n') {
//                 continue;
//             }
//         }
        
//         if (c >= 32 && c <= 126) { // Printable characters
//             line[line_pos++] = c;
//             line_length++;
//         }
//     }
    
//     // Draw remaining text in current line
//     if (line_pos > 0) {
//         line[line_pos] = '\0';
//         draw_string_scaled(x, y, line, COLOR_BLACK, 2);
//     }
    
//     // Draw cursor
//     int cursor_x = 20 + (line_length * 16); // Approximate cursor position
//     int cursor_y = y;
//     fill_rect(cursor_x, cursor_y, 8, 20, COLOR_BLACK);
// }

// // Function to get filename from user
// int get_filename_from_user(char *filename_buffer) {
//     char input_buffer[MAX_FILENAME_LENGTH] = "";
//     int input_pos = 0;
//                     fill_rect(300 + 17*16, 300, 400, 30, COLOR_PASTEL_YELLOW);

//     fill_screen(COLOR_PASTEL_YELLOW);
//     draw_string_scaled(200, 300, "Enter File name: ", COLOR_DIM_GRAY, 3);
    
//     while(1) {
//         if(keyboard_has_input()) {
//             char c = keyboard_getchar();
            
//             if (c == 0x1B) { // ESC to exit
//                 return 0; // Cancel
//             }
//             else if(c == '\n') { // Enter to confirm
//                 if(input_pos > 0) {
//                     mystrcpy(filename_buffer, input_buffer);
//                     return 1; // Success
//                 }
//             }
//             else if(c == '\b') { // Backspace
//                 if(input_pos > 0) {
//                     input_pos--;
//                     input_buffer[input_pos] = '\0';
                    
//                     // Redraw filename input area
//                                         fill_rect(350 + 17*16, 300, 400, 30, COLOR_PASTEL_YELLOW);
//                     draw_string_scaled(350 + 17*16, 300, input_buffer, COLOR_BLACK, 3);
                    
//                                     }
//             }
//             else if(c >= 32 && c <= 126 && input_pos < MAX_FILENAME_LENGTH - 1) { // Printable characters
//                 input_buffer[input_pos] = c;
//                 input_pos++;
//                 input_buffer[input_pos] = '\0';
                
//                 // Redraw filename input area
//                 draw_string_scaled(350 + 17*16, 300, input_buffer, COLOR_BLACK, 3);
                
//                 // Draw cursor
              
//             }
//         }
//     }
// }

// void notepad_loop(const char *default_filename) {
//     // Get filename from user
//     char user_filename[MAX_FILENAME_LENGTH];
    
//     // If a default filename was provided, use it
//     {
//         // Ask user for filename
//         if(!get_filename_from_user(user_filename)) {
//             return; // User pressed ESC
//         }
//     }
    
//     // Store the filename
//     mystrcpy(current_filename, user_filename);
    
//     // Initialize buffer
//     cursor_pos = 0;
//     notepad_buffer[0] = '\0';
//     scroll_offset = 0;
    
//     // Try to load existing file
//     if (file_exists(current_filename)) {
//         int bytes_read = read_files(current_filename, notepad_buffer, NOTEPAD_BUFFER_SIZE - 1);
//         if (bytes_read > 0) {
//             cursor_pos = bytes_read;
//             notepad_buffer[cursor_pos] = '\0';
//         }
//     }
    
//     notepad_draw_interface(current_filename);
//     notepad_draw_text(); // Draw initial text
    
//     while (1) {
//         if (keyboard_has_input()) {
//             char c = keyboard_getchar();
            
//             // Handle Ctrl+S combination using keyboard driver's state
//             if (keyboard_ctrl_pressed() && (c == 's' || c == 'S')) {
//                 // Create file if it doesn't exist
//                 if (!file_exists(current_filename)) {
//                     if (create_files(current_filename) != 0) {
//                         // Show error
//                         fill_rect(fb.width - 250, fb.height - 60, 240, 20, COLOR_DARK_BLUE);
//                         draw_string_scaled(fb.width - 240, fb.height - 55, "Error creating file!", COLOR_RED, 2);
//                         continue;
//                     }
//                 }
                
//                 // Save the file
//                 if (write_files(current_filename, notepad_buffer, cursor_pos) == 0) {
//                     // Show save confirmation
//                     fill_rect(fb.width - 200, fb.height - 60, 190, 20, COLOR_DARK_BLUE);
//                     draw_string_scaled(fb.width - 190, fb.height - 55, "File saved!", COLOR_GREEN, 2);
//                 } else {
//                     // Show error
//                     fill_rect(fb.width - 250, fb.height - 60, 240, 20, COLOR_DARK_BLUE);
//                     draw_string_scaled(fb.width - 240, fb.height - 55, "Error saving file!", COLOR_RED, 2);
//                 }
//                 continue; // Skip normal processing for Ctrl+S
//             }
            
//             if (c == 0x1B) { // ESC to exit
//                 return;
//             }
//             else if (c == '\b') { // Backspace
//                 if (cursor_pos > 0) {
//                     cursor_pos--;
//                     notepad_buffer[cursor_pos] = '\0';
//                     notepad_draw_text();
//                 }
//             }
//             else if (c == '\n') { // Enter key
//                 if (cursor_pos < NOTEPAD_BUFFER_SIZE - 1) {
//                     notepad_buffer[cursor_pos++] = '\n';
//                     notepad_buffer[cursor_pos] = '\0';
//                     notepad_draw_text();
//                 }
//             }
//             else if (c >= 32 && c <= 126) { // Printable characters
//                 if (cursor_pos < NOTEPAD_BUFFER_SIZE - 1) {
//                     notepad_buffer[cursor_pos++] = c;
//                     notepad_buffer[cursor_pos] = '\0';
//                     notepad_draw_text();
//                 }
//             }
//         }
//     }
// }

#include "notepad.h"
#include "../Drivers/vesa.h"
#include "../Drivers/keyboard.h"
#include "../Drivers/rmfs.h"
#include "../Drivers/string.h"
#include "../Drivers/colours.h"

static char notepad_buffer[NOTEPAD_BUFFER_SIZE];
static int cursor_pos = 0;
static int scroll_offset = 0;
static char current_filename[MAX_FILENAME_LENGTH] = "notes.txt";
static int needs_redraw = 1; // Add redraw flag

void notepad_draw_interface(const char *filename) {
    fill_screen(COLOR_BLACK);
    
    // Draw header
    fill_rect(0, 0, fb.width, 30, COLOR_DARK_BLUE);
    draw_string_scaled(10, 5, "VortexOS Notepad", COLOR_WHITE, 2);
    
    // Draw filename
    char title[100];
    mystrcpy(title, "File: ");
    mystrcat(title, filename);
    draw_string_scaled(fb.width - 300, 5, title, COLOR_WHITE, 2);
    
    // Draw instructions
    draw_string_scaled(10, fb.height - 40, "Press ESC to exit, Ctrl+S to save", COLOR_GRAY, 2);
    draw_string_scaled(10, fb.height - 20, "Use arrow keys to navigate, Backspace to delete", COLOR_GRAY, 2);
    
    // Draw text area background
    fill_rect(10, 40, fb.width - 20, fb.height - 80, COLOR_WHITE);
}

void notepad_draw_text(void) {
    // Clear text area
    fill_rect(20, 50, fb.width - 40, fb.height - 100, COLOR_WHITE);
    
    // Draw text with line wrapping
    int x = 20;
    int y = 50;
    int line_length = 0;
    char line[MAX_LINE_LENGTH + 1];
    int line_pos = 0;
    
    for (int i = 0; i < cursor_pos && i < NOTEPAD_BUFFER_SIZE - 1; i++) {
        char c = notepad_buffer[i];
        
        if (c == '\n' || line_length >= MAX_LINE_LENGTH) {
            line[line_pos] = '\0';
            draw_string_scaled(x, y, line, COLOR_BLACK, 2);
            
            y += 24; // Line height
            x = 20;
            line_length = 0;
            line_pos = 0;
            
            if (c == '\n') {
                continue;
            }
        }
        
        if (c >= 32 && c <= 126) { // Printable characters
            line[line_pos++] = c;
            line_length++;
        }
    }
    
    // Draw remaining text in current line
    if (line_pos > 0) {
        line[line_pos] = '\0';
        draw_string_scaled(x, y, line, COLOR_BLACK, 2);
    }
    
    // Draw cursor
    int cursor_x = 20 + (line_length * 16); // Approximate cursor position
    int cursor_y = y;
    fill_rect(cursor_x, cursor_y, 8, 20, COLOR_BLACK);
}

// Function to get filename from user
int get_filename_from_user(char *filename_buffer) {
    char input_buffer[MAX_FILENAME_LENGTH] = "";
    int input_pos = 0;
    fill_rect(300 + 17*16, 300, 400, 30, COLOR_PASTEL_YELLOW);

    fill_screen(COLOR_PASTEL_YELLOW);
    draw_string_scaled(200, 300, "Enter File name: ", COLOR_DIM_GRAY, 3);
    
    while(1) {
        if(keyboard_has_input()) {
            char c = keyboard_getchar();
            
            if (c == 0x1B) { // ESC to exit
                return 0; // Cancel
            }
            else if(c == '\n') { // Enter to confirm
                if(input_pos > 0) {
                    mystrcpy(filename_buffer, input_buffer);
                    return 1; // Success
                }
            }
            else if(c == '\b') { // Backspace
                if(input_pos > 0) {
                    input_pos--;
                    input_buffer[input_pos] = '\0';
                    
                    // Redraw filename input area
                    fill_rect(350 + 17*16, 300, 400, 30, COLOR_PASTEL_YELLOW);
                    draw_string_scaled(350 + 17*16, 300, input_buffer, COLOR_BLACK, 3);
                }
            }
            else if(c >= 32 && c <= 126 && input_pos < MAX_FILENAME_LENGTH - 1) { // Printable characters
                input_buffer[input_pos] = c;
                input_pos++;
                input_buffer[input_pos] = '\0';
                
                // Redraw filename input area
                draw_string_scaled(350 + 17*16, 300, input_buffer, COLOR_BLACK, 3);
            }
        }
    }
}

void notepad_loop(const char *default_filename) {
    // Get filename from user
    char user_filename[MAX_FILENAME_LENGTH];
    
    // If a default filename was provided, use it
    {
        // Ask user for filename
        if(!get_filename_from_user(user_filename)) {
            return; // User pressed ESC
        }
    }
    
    // Store the filename
    mystrcpy(current_filename, user_filename);
    
    // Initialize buffer
    cursor_pos = 0;
    notepad_buffer[0] = '\0';
    scroll_offset = 0;
    needs_redraw = 1; // Initialize redraw flag
    
    // Try to load existing file
    if (file_exists(current_filename)) {
        int bytes_read = read_files(current_filename, notepad_buffer, NOTEPAD_BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            cursor_pos = bytes_read;
            notepad_buffer[cursor_pos] = '\0';
        }
    }
    
    notepad_draw_interface(current_filename);
    notepad_draw_text(); // Draw initial text
    
    while (1) {
        // Process all buffered key presses first
        while (keyboard_has_input()) {
            char c = keyboard_getchar();
            
            // Handle Ctrl+S combination using keyboard driver's state
            if (keyboard_ctrl_pressed() && (c == 's' || c == 'S')) {
                // Create file if it doesn't exist
                if (!file_exists(current_filename)) {
                    if (create_files(current_filename) != 0) {
                        // Show error
                        fill_rect(fb.width - 250, fb.height - 60, 240, 20, COLOR_DARK_BLUE);
                        draw_string_scaled(fb.width - 240, fb.height - 55, "Error creating file!", COLOR_RED, 2);
                        continue;
                    }
                }
                
                // Save the file
                if (write_files(current_filename, notepad_buffer, cursor_pos) == 0) {
                    // Show save confirmation
                    fill_rect(fb.width - 200, fb.height - 60, 190, 20, COLOR_DARK_BLUE);
                    draw_string_scaled(fb.width - 190, fb.height - 55, "File saved!", COLOR_GREEN, 2);
                } else {
                    // Show error
                    fill_rect(fb.width - 250, fb.height - 60, 240, 20, COLOR_DARK_BLUE);
                    draw_string_scaled(fb.width - 240, fb.height - 55, "Error saving file!", COLOR_RED, 2);
                }
                continue; // Skip normal processing for Ctrl+S
            }
            
            if (c == 0x1B) { // ESC to exit
                return;
            }
            else if (c == '\b') { // Backspace
                if (cursor_pos > 0) {
                    cursor_pos--;
                    notepad_buffer[cursor_pos] = '\0';
                    needs_redraw = 1; // Set flag instead of immediate redraw
                }
            }
            else if (c == '\n') { // Enter key
                if (cursor_pos < NOTEPAD_BUFFER_SIZE - 1) {
                    notepad_buffer[cursor_pos++] = '\n';
                    notepad_buffer[cursor_pos] = '\0';
                    needs_redraw = 1; // Set flag instead of immediate redraw
                }
            }
            else if (c >= 32 && c <= 126) { // Printable characters
                if (cursor_pos < NOTEPAD_BUFFER_SIZE - 1) {
                    notepad_buffer[cursor_pos++] = c;
                    notepad_buffer[cursor_pos] = '\0';
                    needs_redraw = 1; // Set flag instead of immediate redraw
                }
            }
        }
        
        // Only redraw if needed (after processing all keys)
        if (needs_redraw) {
            notepad_draw_text();
            needs_redraw = 0; // Reset flag
        }
        
        // Halt CPU when no input to reduce CPU usage
        asm volatile("hlt");
    }
}