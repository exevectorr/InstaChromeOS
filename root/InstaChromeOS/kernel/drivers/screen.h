#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

/* VGA colors */
enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

/* Function declarations */
void init_screen(void);
void screen_clear(void);
void screen_write(const char* str);
void screen_write_char(char c);
void screen_set_color(uint8_t fg, uint8_t bg);
void screen_set_cursor(int x, int y);
void int_to_str(int num, char* str);

#endif