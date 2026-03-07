#include "keyboard.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include <stddef.h>

/* Key state tracking */
static uint8_t key_states[128] = {0};

/* Special key states */
static int shift_pressed = 0;
static int caps_lock_on = 0;
static keyboard_layout_t current_layout = LAYOUT_US_ENGLISH;

/* US English Keyboard layout (QWERTY) */
static const char keyboard_map_us_lower[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char keyboard_map_us_upper[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

/* Swedish Keyboard layout (QWERTY) */
static const char keyboard_map_swedish_lower[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '+', '\'', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'å', '¨', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö', 'ä', '\'', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, '*', 0, ' '
};

static const char keyboard_map_swedish_upper[] = {
    0, 0, '!', '"', '#', '¤', '%', '&', '/', '(', ')', '=', '?', '`', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'Å', '^', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Ö', 'Ä', '*', 0,
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0, '*', 0, ' '
};

/* Port I/O functions */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Check if keyboard has data */
static int keyboard_has_data(void) {
    return (inb(0x64) & 1) != 0;
}

/* Initialize keyboard */
void init_keyboard(void) {
    shift_pressed = 0;
    caps_lock_on = 0;
    current_layout = LAYOUT_US_ENGLISH;
    
    /* Clear key states */
    for(int i = 0; i < 128; i++) {
        key_states[i] = 0;
    }
}

/* Set keyboard layout */
void keyboard_set_layout(keyboard_layout_t layout) {
    current_layout = layout;
}

/* Get current keyboard layout */
keyboard_layout_t keyboard_get_layout(void) {
    return current_layout;
}

/* Get layout name as string */
const char* keyboard_get_layout_name(void) {
    switch(current_layout) {
        case LAYOUT_US_ENGLISH:
            return "US English";
        case LAYOUT_SWEDISH:
            return "Swedish";
        default:
            return "Unknown";
    }
}

/* Get character from keyboard with layout support */
char keyboard_getchar(void) {
    uint8_t scancode;
    char c = 0;
    
    while(1) {
        /* Wait for data */
        while(!keyboard_has_data());
        
        /* Read scancode */
        scancode = inb(0x60);
        
        /* Update key state */
        if(scancode < 128) {
            key_states[scancode] = !(scancode & 0x80);
        }
        
        /* Handle special keys */
        switch(scancode) {
            /* Left Shift press/release */
            case 0x2A: shift_pressed = 1; continue;
            case 0xAA: shift_pressed = 0; continue;
            
            /* Right Shift press/release */
            case 0x36: shift_pressed = 1; continue;
            case 0xB6: shift_pressed = 0; continue;
            
            /* Caps Lock press (toggle) */
            case 0x3A: 
                caps_lock_on = !caps_lock_on;
                continue;
        }
        
        /* Only handle key presses (not releases) */
        if(!(scancode & 0x80) && scancode < 58) {  /* Only first 58 scancodes are valid */
            /* Determine if we should use uppercase */
            int use_upper = (shift_pressed && !caps_lock_on) || (!shift_pressed && caps_lock_on);
            
            /* Get character based on current layout */
            if(current_layout == LAYOUT_US_ENGLISH) {
                if(use_upper) {
                    c = keyboard_map_us_upper[scancode];
                } else {
                    c = keyboard_map_us_lower[scancode];
                }
            } else if(current_layout == LAYOUT_SWEDISH) {
                if(use_upper) {
                    c = keyboard_map_swedish_upper[scancode];
                } else {
                    c = keyboard_map_swedish_lower[scancode];
                }
            }
            
            if(c) {
                return c;
            }
        }
    }
}

/* Get shift state */
int keyboard_get_shift_state(void) {
    return shift_pressed;
}

/* Get caps lock state */
int keyboard_get_caps_state(void) {
    return caps_lock_on;
}