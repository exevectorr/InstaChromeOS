#include "bios.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include "../kernel.h"

/* BIOS state */
static int in_bios = 0;

/* BIOS colors - use values from screen.h */
#define BIOS_BG COLOR_BLUE
#define BIOS_FG COLOR_WHITE
#define BIOS_HIGHLIGHT COLOR_LIGHT_GREY
#define BIOS_BORDER COLOR_CYAN

/* Display the initial BIOS screen for 2.5 seconds */
void display_bios_screen(void) {
    /* Save current screen state */
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_clear();
    
    char buffer[64];
    
    /* Top border */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" === APM 1.2 ");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("======================================================");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" SVN 9458 ");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("====\n");
    
    /* Empty line with borders */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("                                                                              ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* InstaChromeOS version line */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" InstaChromeOS  version 0.0.4.3+      ");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("[auto-build 7 mars 2026 16:33, h2875]   ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Separator line */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("==============================================================================");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Copyright line */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" InstaChromeBIOS All rights reserved(C) 2026                                  ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Empty lines */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("                                                                              ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("                                                                              ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* BIOS key hint */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write("  Press right shift+F12 to toggle BIOS                                        ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Setup key hint */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write("  Press right shift and F10 to toggle InstaChromeCHX                          ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Empty line */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("                                                                              ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Copyright footer */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" [");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write("  All rights reserved (c) InstaChromeOS 2026                                  ");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write("]\n");
    
    /* Bottom border */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" ===============================================================================\n");
    
    /* Reset color */
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    /* Wait 2.5 seconds */
    for(volatile int i = 0; i < 1000000000; i++);
}

/* Check if BIOS key combination is pressed */
int check_bios_key(void) {
    /* This would need proper keyboard driver support */
    /* For now, always return 0 */
    return 0;
}

/* Display BIOS setup menu */
void bios_menu(void) {
    in_bios = 1;
    
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_clear();
    
    char buffer[64];
    
    /* Title */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" ==============================================================================\n");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write("                           InstaChromeOS BIOS Setup                            \n");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" ==============================================================================\n\n");
    
    /* Menu options */
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write(" Main     Advanced     Security     Boot     Exit\n\n");
    
    /* Settings */
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" System Information:\n");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("   BIOS Version    : APM 1.2 (SVN 9458)\n");
    screen_write("   Build Date      : 7 mars 2026 16:33\n");
    screen_write("   Build Number    : h2875\n");
    screen_write("   CPU Type        : i686\n");
    screen_write("   Total Memory    : ");
    
    int_to_str(pmm_get_total_memory() / 1048576, buffer);
    screen_write(buffer);
    screen_write(" MB (");
    int_to_str(pmm_get_total_pages(), buffer);
    screen_write(buffer);
    screen_write(" pages)\n\n");
    
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" Boot Options:\n");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("   [X] Boot from CD/DVD\n");
    screen_write("   [ ] Boot from USB\n");
    screen_write("   [X] Boot from HDD\n");
    screen_write("   [ ] Boot from Network\n\n");
    
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" Advanced Settings:\n");
    screen_set_color(BIOS_FG, BIOS_BG);
    screen_write("   CPU Cores        : 1\n");
    screen_write("   ACPI             : Enabled\n");
    screen_write("   APIC             : Disabled\n");
    screen_write("   HPET             : Disabled\n\n");
    
    /* Navigation help */
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" ------------------------------------------------------------------------------\n");
    screen_set_color(BIOS_HIGHLIGHT, BIOS_BG);
    screen_write(" F10: Save & Exit    ESC: Exit    F9: Load Defaults    Arrow Keys: Navigate\n");
    screen_set_color(BIOS_BORDER, BIOS_BG);
    screen_write(" ==============================================================================\n");
    
    /* Wait for key press */
    while(in_bios) {
        char c = keyboard_getchar();
        
        if(c == 27) {  /* ESC key */
            in_bios = 0;
            screen_clear();
            screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        }
    }
}

/* Enter BIOS setup */
void enter_bios(void) {
    bios_menu();
}

/* Check if we're in BIOS mode */
int is_in_bios(void) {
    return in_bios;
}