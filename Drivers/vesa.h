// vesa.h
//uint32_t,16_t,8_t means giving a fix size to int because int could be of 32,64.. this helps in memory management
//sttribute packed ki padding bytes add na ho compiler ki tarafse
//all the struct are on the basis of vbe[vesa] standards 
// vesa.h
#ifndef VESA_H
#define VESA_H

#include <stdint.h>

uint32_t get_multiboot_info(uint32_t magic, uint32_t *info);

// VBE Info Block structure
struct vbe_info_block {
    char signature[4];
    uint16_t version;
    uint32_t oem_string_ptr;
    uint32_t capabilities;
    uint32_t video_mode_ptr;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name_ptr;
    uint32_t oem_product_name_ptr;
    uint32_t oem_product_rev_ptr;
    char reserved[222];
    char oem_data[256];
} __attribute__((packed));

// VBE Mode Info structure
struct vbe_mode_info {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed));

// Framebuffer structure
struct framebuffer {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint8_t *address;
    uint32_t size;
};

extern struct framebuffer fb;

// Font size control
void set_font_size(uint8_t size);
uint8_t get_font_size(void);
uint32_t get_char_width(void);
uint32_t get_char_height(void);

void init_vesa(uint32_t magic, uint32_t mb_info);
void fill_screen(uint32_t color);
void set_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_char(uint32_t x, uint32_t y, char c, uint32_t color);
void draw_string(uint32_t x, uint32_t y, const char* str, uint32_t color);
void draw_char_scaled(uint32_t x, uint32_t y, char c, uint32_t color, uint8_t scale);
void draw_string_scaled(uint32_t x, uint32_t y, const char* str, uint32_t color, uint8_t scale);

// Add these to vesa.h
uint32_t interpolate_color(uint32_t color1, uint32_t color2, uint32_t step, uint32_t total_steps);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
#endif