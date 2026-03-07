#include "keyboard.h"
#include "../drivers/screen.h"
#include <stddef.h>

/* Keyboard buffer for future use */
static char key_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;

/* US Keyboard layout with both lowercase and uppercase */
static const char keyboard_map_lower[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char keyboard_map_upper[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

/* Special key states */
static int shift_pressed = 0;
static int caps_lock_on = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

/* Port I/O functions */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Check if keyboard has data */
static int keyboard_has_data(void) {
    return (inb(0x64) & 1) != 0;
}

/* Initialize keyboard */
void init_keyboard(void) {
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = 0;
    caps_lock_on = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    
    /* Set keyboard LEDs (optional) */
    /* Wait for keyboard to be ready */
    while (inb(0x64) & 2);
    /* Send command to set LEDs */
    outb(0x60, 0xED);
    /* Wait for ACK */
    while (!(inb(0x64) & 1));
    /* Read ACK (should be 0xFA) */
    inb(0x60);
    /* Set LEDs (caps lock off) */
    outb(0x60, 0x00);
}

/* Get character from keyboard with uppercase support */
char keyboard_getchar(void) {
    uint8_t scancode;
    char c = 0;
    
    while(1) {
        /* Wait for data */
        while(!keyboard_has_data());
        
        /* Read scancode */
        scancode = inb(0x60);
        
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
                /* Update Caps Lock LED */
                while (inb(0x64) & 2);
                outb(0x60, 0xED);
                while (!(inb(0x64) & 1));
                inb(0x60);
                outb(0x60, caps_lock_on ? 0x04 : 0x00);
                continue;
            
            /* Ctrl keys */
            case 0x1D: ctrl_pressed = 1; continue;
            case 0x9D: ctrl_pressed = 0; continue;
            
            /* Alt keys */
            case 0x38: alt_pressed = 1; continue;
            case 0xB8: alt_pressed = 0; continue;
        }
        
        /* Only handle key presses (not releases) */
        if(!(scancode & 0x80) && scancode < sizeof(keyboard_map_lower)) {
            /* Determine if we should use uppercase */
            int use_upper = (shift_pressed && !caps_lock_on) || (!shift_pressed && caps_lock_on);
            
            if(use_upper) {
                c = keyboard_map_upper[scancode];
            } else {
                c = keyboard_map_lower[scancode];
            }
            
            /* Handle Ctrl combinations (optional) */
            if(ctrl_pressed && c >= 'a' && c <= 'z') {
                c = c - 'a' + 1;  /* Convert to control character */
            }
            
            if(c) {
                return c;
            }
        }
    }
}

/* Optional: Function to get shift state */
int keyboard_get_shift_state(void) {
    return shift_pressed;
}

/* Optional: Function to get caps lock state */
int keyboard_get_caps_state(void) {
    return caps_lock_on;
}