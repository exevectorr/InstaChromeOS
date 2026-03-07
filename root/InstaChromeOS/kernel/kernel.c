#include "kernel.h"
#include "boot/multiboot.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "mm/pmm.h"
#include "fs/fat32.h"
#include "lib/string.h"  // Added missing include

/* Kernel main function */
void kernel_main(uint32_t magic, uint32_t addr) {
    /* Initialize subsystems */
    init_screen();
    init_memory();
    init_keyboard();
    init_fs();
    
    /* Display memory info for 3 seconds */
    display_memory_info();
    
    /* Display welcome screen and start shell */
    display_welcome();
    start_shell();
    
    /* Should never reach here */
    while(1) {
        __asm__ volatile("hlt");
    }
}

/* Memory initialization */
void init_memory(void) {
    /* Initialize physical memory manager */
    pmm_init();
}

/* Display memory information for 3 seconds */
void display_memory_info(void) {
    memory_info_t mem;
    
    /* Get memory info from PMM */
    mem.total_mb = pmm_get_total_memory() / 1048576;  /* Convert to MB */
    mem.used_mb = pmm_get_used_memory() / 1048576;
    mem.free_kb = (pmm_get_total_memory() - pmm_get_used_memory()) / 1024;
    
    screen_clear();
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    screen_write("\n\n");
    screen_write("========================================\n");
    screen_write("      MEMORY INFORMATION\n");
    screen_write("========================================\n");
    
    char buffer[64];
    screen_write("memory used......................");
    int_to_str(mem.used_mb, buffer);
    screen_write(buffer);
    screen_write(" MB\n");
    
    screen_write("memory whole......................");
    int_to_str(mem.total_mb, buffer);
    screen_write(buffer);
    screen_write(" MB\n");
    
    screen_write("                ");
    int_to_str(mem.free_kb, buffer);
    screen_write(buffer);
    screen_write(" KB memory free\n");
    
    screen_write("========================================\n");
    
    /* Wait 3 seconds (simple delay loop) */
    for(volatile int i = 0; i < 300000000; i++);
}

/* Display welcome screen */
void display_welcome(void) {
    screen_clear();
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    screen_write("\n\n");
    screen_write("==========================================\n");
    screen_write("         welcome to InstaChromeOS v1.0\n");
    screen_write("==========================================\n");
    screen_write("                  Powerd by InstaChrome\n");
    screen_write("\n");
    screen_write("root> ");
}

/* Shell implementation */
void start_shell(void) {
    char command[256];
    char current_dir[256] = "/";
    int cmd_index = 0;
    
    while(1) {
        char c = keyboard_getchar();
        
        if(c == '\n') {
            command[cmd_index] = '\0';
            
            if(strcmp(command, "dir") == 0) {
                fs_print_tree();
            }
            else if(strcmp(command, "lst") == 0) {
                fs_list_current();
            }
            else if(strncmp(command, "cd ", 3) == 0) {
                fs_change_dir(command + 3);
            }
            else if(strncmp(command, "tayn ", 5) == 0) {
                fs_create_file(command + 5);
            }
            else if(strncmp(command, "mkdir ", 6) == 0) {
                fs_create_dir(command + 6);
            }
            else if(strncmp(command, "printf * ", 9) == 0) {
                screen_write(command + 9);
                screen_write("\n");
            }
            else if(strcmp(command, "clear") == 0) {
                screen_clear();
            }
            else if(strcmp(command, "help") == 0) {
                screen_write("\nAvailable commands:\n");
                screen_write("  dir              - Display full filesystem\n");
                screen_write("  lst              - List current directory\n");
                screen_write("  cd <dir>         - Change directory\n");
                screen_write("  tayn <file>      - Create new file\n");
                screen_write("  mkdir <dir>      - Create new directory\n");
                screen_write("  printf * <text>  - Print text\n");
                screen_write("  clear            - Clear screen\n");
                screen_write("  help             - Show this help\n");
                screen_write("\n");
            }
            else if(cmd_index > 0) {
                screen_write("Command not found. Type 'help' for available commands.\n");
            }
            
            fs_get_path(current_dir, sizeof(current_dir));
            screen_write(current_dir);
            screen_write("> ");
            cmd_index = 0;
        }
        else if(c == '\b' && cmd_index > 0) {
            cmd_index--;
            screen_write("\b \b");
        }
        else if(c >= ' ' && c <= '~' && cmd_index < 255) {
            command[cmd_index++] = c;
            char str[2] = {c, '\0'};
            screen_write(str);
        }
    }
}