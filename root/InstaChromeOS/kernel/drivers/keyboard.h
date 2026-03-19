#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* Keyboard layout types */
typedef enum {
    LAYOUT_US_ENGLISH,
    LAYOUT_SWEDISH
} keyboard_layout_t;

/* Special key codes - these are returned by keyboard_getchar() for non-ASCII keys */
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

/* Scancode definitions for keyboard_is_key_pressed */
#define SCANCODE_ESC         0x01
#define SCANCODE_1           0x02
#define SCANCODE_2           0x03
#define SCANCODE_3           0x04
#define SCANCODE_4           0x05
#define SCANCODE_5           0x06
#define SCANCODE_6           0x07
#define SCANCODE_7           0x08
#define SCANCODE_8           0x09
#define SCANCODE_9           0x0A
#define SCANCODE_0           0x0B
#define SCANCODE_MINUS       0x0C
#define SCANCODE_EQUALS      0x0D
#define SCANCODE_BACKSPACE   0x0E
#define SCANCODE_TAB         0x0F
#define SCANCODE_Q           0x10
#define SCANCODE_W           0x11
#define SCANCODE_E           0x12
#define SCANCODE_R           0x13
#define SCANCODE_T           0x14
#define SCANCODE_Y           0x15
#define SCANCODE_U           0x16
#define SCANCODE_I           0x17
#define SCANCODE_O           0x18
#define SCANCODE_P           0x19
#define SCANCODE_LEFTBRACKET 0x1A
#define SCANCODE_RIGHTBRACKET 0x1B
#define SCANCODE_ENTER       0x1C
#define SCANCODE_LCTRL       0x1D
#define SCANCODE_A           0x1E
#define SCANCODE_S           0x1F
#define SCANCODE_D           0x20
#define SCANCODE_F           0x21
#define SCANCODE_G           0x22
#define SCANCODE_H           0x23
#define SCANCODE_J           0x24
#define SCANCODE_K           0x25
#define SCANCODE_L           0x26
#define SCANCODE_SEMICOLON   0x27
#define SCANCODE_APOSTROPHE  0x28
#define SCANCODE_GRAVE       0x29
#define SCANCODE_LSHIFT      0x2A
#define SCANCODE_BACKSLASH   0x2B
#define SCANCODE_Z           0x2C
#define SCANCODE_X           0x2D
#define SCANCODE_C           0x2E
#define SCANCODE_V           0x2F
#define SCANCODE_B           0x30
#define SCANCODE_N           0x31
#define SCANCODE_M           0x32
#define SCANCODE_COMMA       0x33
#define SCANCODE_PERIOD      0x34
#define SCANCODE_SLASH       0x35
#define SCANCODE_RSHIFT      0x36
#define SCANCODE_KP_MULTIPLY 0x37
#define SCANCODE_LALT        0x38
#define SCANCODE_SPACE       0x39
#define SCANCODE_CAPSLOCK    0x3A
#define SCANCODE_F1          0x3B
#define SCANCODE_F2          0x3C
#define SCANCODE_F3          0x3D
#define SCANCODE_F4          0x3E
#define SCANCODE_F5          0x3F
#define SCANCODE_F6          0x40
#define SCANCODE_F7          0x41
#define SCANCODE_F8          0x42
#define SCANCODE_F9          0x43
#define SCANCODE_F10         0x44
#define SCANCODE_NUMLOCK     0x45
#define SCANCODE_SCROLLLOCK  0x46
#define SCANCODE_HOME        0x47
#define SCANCODE_UP          0x48
#define SCANCODE_PAGEUP      0x49
#define SCANCODE_KP_MINUS    0x4A
#define SCANCODE_LEFT        0x4B
#define SCANCODE_KP_5        0x4C
#define SCANCODE_RIGHT       0x4D
#define SCANCODE_KP_PLUS     0x4E
#define SCANCODE_END         0x4F
#define SCANCODE_DOWN        0x50
#define SCANCODE_PAGEDOWN    0x51
#define SCANCODE_INSERT      0x52
#define SCANCODE_DELETE      0x53
#define SCANCODE_F11         0x57
#define SCANCODE_F12         0x58

/* Function declarations */
void init_keyboard(void);
int keyboard_getchar(void);  /* Returns ASCII or special key codes (KEY_F1 etc) */
int keyboard_get_shift_state(void);
int keyboard_get_caps_state(void);
void keyboard_set_layout(keyboard_layout_t layout);
keyboard_layout_t keyboard_get_layout(void);
const char* keyboard_get_layout_name(void);
int keyboard_is_key_pressed(uint8_t scancode);  /* Check if a specific key is currently pressed */

#endif /* KEYBOARD_H */