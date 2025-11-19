#include "Drivers/vesa.h"
#include "Interrupts/pic.h"
#include "Interrupts/interrupts.h"
#include "Interrupts/gdt.h"
#include "Interrupts/tss.h"
#include "Drivers/keyboard.h"
#include "application/shell.h"
#include "snakegame.h"
#include "Interrupts/timer.h"
#include "Drivers/rmfs.h"
#include "application/notepad.h"
#include "Drivers/colours.h"
extern uint32_t stack_top;
void draw_dashboard(void);
int handle_dashboard_input(void);
void write_string(int color, const char* str) {
    volatile char* video_memory = (volatile char*)0xB8000;
    while(*str) {
        *video_memory = *str;
        video_memory++;
        *video_memory = color;
        video_memory++;
        str++;
    }
}

void draw_snake_icon(int x, int y) {
    uint32_t bg_color = 0xFF1a3a1a;    // Dark green
    uint32_t snake_color = 0xFF00FF00; // Bright green
    uint32_t food_color = 0xFFFF0000;  // Red
    
    // Game board background
    fill_rect(x, y, 48, 36, bg_color);
    
    // Snake body
    fill_rect(x + 10, y + 18, 12, 4, snake_color);
    fill_rect(x + 18, y + 14, 4, 8, snake_color);
    fill_rect(x + 22, y + 14, 8, 4, snake_color);
    
    // Snake head
    fill_rect(x + 30, y + 14, 6, 4, 0xFF80FF80); // Lighter green for head
    
    // Food
    fill_rect(x + 35, y + 8, 4, 4, food_color);
}

void draw_system_icon(int x, int y) {
    uint32_t bg_color = 0xFF3a1a3a;    // Dark purple
    uint32_t accent_color = 0xFFFFA500; // Orange
    
    // Computer tower
    fill_rect(x + 15, y + 8, 18, 28, bg_color);
    
    // Details
    fill_rect(x + 18, y + 12, 12, 4, accent_color);  // LED
    fill_rect(x + 17, y + 20, 14, 2, 0xFF666666);    // Drive slot
    fill_rect(x + 17, y + 25, 14, 2, 0xFF666666);    // Drive slot
    
    // Monitor
    fill_rect(x + 8, y + 5, 32, 20, 0xFF111111);
    fill_rect(x + 10, y + 7, 28, 16, 0xFF0066CC);
    
    // Stand
    fill_rect(x + 21, y + 25, 6, 8, 0xFF444444);}

void draw_shell_icon(int x, int y) {
    uint32_t bg_color = 0xFF2a2a2a;    // Dark gray
    uint32_t text_color = 0xFF00FF00;  // Green text
    
    // Terminal window
    fill_rect(x, y, 50, 40, bg_color);
    
    // Terminal header
    fill_rect(x, y, 50, 8, 0xFF555555);
    
    // Terminal prompt
    draw_string(x + 5, y + 15, ">", text_color);
    draw_string(x + 15, y + 15, "shell", 0xFF00AAFF);
        fill_rect(x + 45, y + 15, 3, 8, text_color);
}
void draw_notepad_icon(int x, int y) {
    uint32_t paper_color = 0xFFFFFFFF;  // White paper
    uint32_t cover_color = 0xFF1E90FF;  // Dodger blue cover
        fill_rect(x, y, 50, 40, cover_color);
        fill_rect(x + 5, y + 5, 45, 35, paper_color);
        for (int i = 0; i < 4; i++) {
        fill_rect(x + 10, y + 15 + i * 6, 35, 2, 0xFFDDDDDD);
    }
        fill_rect(x, y, 3, 40, 0xFF666666);
}
void draw_icon_card(uint32_t x, uint32_t y, const char* title, uint32_t base_color, void (*draw_icon_func)(int, int)) {
    fill_rect(x-10, y-10, 140, 160, COLOR_ALMOST_BLACK);
    fill_rect(x-5, y-5, 130, 150, base_color);
    fill_rect(x, y, 120, 140, COLOR_SLATE_GRAY);
        if (draw_icon_func) {
        draw_icon_func(x + 35, y + 30); // Center the icon (120-50=70/2=35, 140-80=60/2=30)
    }
        uint32_t title_len = 0;
    const char* temp = title;
    while (*temp++) title_len++;
    uint32_t text_x = x + (120 - title_len * 8) / 2; // Center text based on character count
    draw_string_scaled(text_x, y + 105, title, COLOR_CHARCOAL,1);
}

void draw_dashboard(void) {
    for (uint32_t y = 0; y < fb.height; y++) {
        uint32_t color = interpolate_color(COLOR_NAVY, COLOR_PURPLE, y, fb.height);
                uint32_t *row_start = (uint32_t*)(fb.address + (y * fb.pitch));
                for (uint32_t x = 0; x < fb.width; x++) {
            row_start[x] = color;
        }
    }
        draw_string_scaled(225, 45, "VortexOS Dashboard", COLOR_CHARCOAL, 4);
    draw_string_scaled(230, 50, "VortexOS Dashboard", COLOR_GOLD, 4);
        draw_icon_card(260, 150, "Shell", COLOR_DIM_GRAY, draw_shell_icon);
    draw_icon_card(460, 150, "Snake Game", COLOR_SEA, draw_snake_icon);
    draw_icon_card(660, 150, "Notepad", COLOR_NAVY, draw_notepad_icon);
        draw_string_scaled(280, 470, "PRESS S=SHELL  G=GAME  N=NOTEPAD", COLOR_PASTEL_YELLOW, 2);
}
void kernel_main(uint32_t magic, uint32_t mb_info) {
    init_gdt();
    init_tss();
    init_idt();
    pic_remap();
        init_keyboard();
init_timer(100);
rmfs_init();
    for (int i = 0; i < 16; i++) {
        {pic_set_mask(i);}
    }
    pic_clear_mask(0);
    pic_clear_mask(1);
    asm volatile("sti");
    init_vesa(magic, mb_info);
        draw_dashboard();    //draw_string(300, 300, "Hello VortexOS",  0xFF000000); // White text
    int y_pos = 320;
    int x_pos=300;
    while (1) {
        if (keyboard_has_input()) {
            char c = keyboard_getchar();
            if (c == 's' || c == 'S') {
                shell_loop();
                draw_dashboard();
            }
            else if(c=='g'||c=='G')
            {
                snake_game_loop();
                draw_dashboard();
            }
            else if(c=='N'|| c=='n')
            {
                notepad_loop("notes.txt");
                draw_dashboard();
            }
            else if (c == 'q' || c == 'Q') {
                // Quit - you could add other applications here
                fill_screen(0x00FFFFFF);
                draw_string(400, 384, "Press 's' for Shell or other keys for apps", 0xFF000000);
            }
        }
    }
}