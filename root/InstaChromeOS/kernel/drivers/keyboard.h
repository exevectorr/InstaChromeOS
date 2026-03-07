#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* Keyboard layout types */
typedef enum {
    LAYOUT_US_ENGLISH,
    LAYOUT_SWEDISH
} keyboard_layout_t;

/* Special key codes */
#define KEY_ESC         27
#define KEY_ENTER       '\n'
#define KEY_BACKSPACE   '\b'
#define KEY_TAB         '\t'
#define KEY_F1          0x80
#define KEY_F2          0x81
#define KEY_F3          0x82
#define KEY_F4          0x83
#define KEY_F5          0x84
#define KEY_F6          0x85
#define KEY_F7          0x86
#define KEY_F8          0x87
#define KEY_F9          0x88
#define KEY_F10         0x89
#define KEY_F11         0x8A
#define KEY_F12         0x8B
#define KEY_UP          0x8C
#define KEY_DOWN        0x8D
#define KEY_LEFT        0x8E
#define KEY_RIGHT       0x8F
#define KEY_HOME        0x90
#define KEY_END         0x91
#define KEY_PAGEUP      0x92
#define KEY_PAGEDOWN    0x93
#define KEY_INSERT      0x94
#define KEY_DELETE      0x95

/* Function declarations */
void init_keyboard(void);
int keyboard_getchar(void);  /* Returns special key codes for F-keys */
int keyboard_get_shift_state(void);
int keyboard_get_caps_state(void);
void keyboard_set_layout(keyboard_layout_t layout);
keyboard_layout_t keyboard_get_layout(void);
const char* keyboard_get_layout_name(void);
int keyboard_is_key_pressed(uint8_t scancode);

#endif