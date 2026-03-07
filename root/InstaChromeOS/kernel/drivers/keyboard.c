#include "keyboard.h"
#include "../drivers/screen.h"
#include <stddef.h>

/* Simple keyboard for testing */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int keyboard_has_data(void) {
    return (inb(0x64) & 1) != 0;
}

void init_keyboard(void) {
    /* Nothing to initialize */
}

char keyboard_getchar(void) {
    uint8_t scancode;
    
    /* Simple scancode to ASCII mapping for common keys */
    static const char scancode_to_ascii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
        '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };
    
    while(1) {
        while(!keyboard_has_data());
        
        scancode = inb(0x60);
        
        /* Only return on key press (not release) */
        if(!(scancode & 0x80) && scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            if(c) return c;
        }
    }
}