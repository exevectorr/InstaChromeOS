#include "screen.h"

/* VGA buffer location */
static uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0;

/* Create VGA entry */
static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

/* Initialize screen */
void init_screen(void) {
    screen_clear();
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}

/* Clear screen */
void screen_clear(void) {
    for(int y = 0; y < VGA_HEIGHT; y++) {
        for(int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', current_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    screen_set_cursor(0, 0);
}

/* Write string */
void screen_write(const char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        screen_write_char(str[i]);
    }
}

/* Write character */
void screen_write_char(char c) {
    if(c == '\n') {
        cursor_x = 0;
        cursor_y++;
    }
    else if(c == '\b') {
        if(cursor_x > 0) {
            cursor_x--;
            VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = vga_entry(' ', current_color);
        }
    }
    else {
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = vga_entry(c, current_color);
        cursor_x++;
    }
    
    /* Handle line wrapping */
    if(cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    /* Scroll if needed */
    if(cursor_y >= VGA_HEIGHT) {
        /* Move all lines up */
        for(int y = 1; y < VGA_HEIGHT; y++) {
            for(int x = 0; x < VGA_WIDTH; x++) {
                VGA_BUFFER[(y-1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
            }
        }
        
        /* Clear last line */
        for(int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(VGA_HEIGHT-1) * VGA_WIDTH + x] = vga_entry(' ', current_color);
        }
        
        cursor_y = VGA_HEIGHT - 1;
    }
    
    screen_set_cursor(cursor_x, cursor_y);
}

/* Set color */
void screen_set_color(uint8_t fg, uint8_t bg) {
    current_color = fg | bg << 4;
}

/* Set cursor position */
void screen_set_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    
    /* Send cursor position to VGA controller */
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x0F), "d"((uint16_t)0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)(pos & 0xFF)), "d"((uint16_t)0x3D5));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x0E), "d"((uint16_t)0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)((pos >> 8) & 0xFF)), "d"((uint16_t)0x3D5));
    
    cursor_x = x;
    cursor_y = y;
}

/* Convert integer to string */
void int_to_str(int num, char* str) {
    int i = 0;
    int is_negative = 0;
    
    if(num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while(num > 0);
    
    if(is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    /* Reverse string */
    for(int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}