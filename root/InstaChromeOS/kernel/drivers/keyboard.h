#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* Keyboard layout types */
typedef enum {
    LAYOUT_US_ENGLISH,
    LAYOUT_SWEDISH
} keyboard_layout_t;

/* Function declarations */
void init_keyboard(void);
char keyboard_getchar(void);
int keyboard_get_shift_state(void);
int keyboard_get_caps_state(void);
void keyboard_set_layout(keyboard_layout_t layout);
keyboard_layout_t keyboard_get_layout(void);
const char* keyboard_get_layout_name(void);

#endif