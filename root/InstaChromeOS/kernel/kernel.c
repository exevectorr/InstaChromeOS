#include "kernel.h"
#include "boot/multiboot.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "mm/pmm.h"
#include "fs/fat32.h"
#include "lib/string.h"
#include "bios/bios.h"

/* External symbols from linker script */
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

/* Kernel main function */
void kernel_main(uint32_t magic, uint32_t addr) {
    /* Suppress unused parameter warnings */
    (void)magic;
    (void)addr;
    
    /* Initialize subsystems */
    init_screen();
    init_memory();
    init_keyboard();
    init_fs();
    
    /* Display BIOS screen for 2.5 seconds */
    display_bios_screen();
    
    /* Display memory info for 5 seconds */
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

/* Helper function to convert integer to hex string */
void int_to_hex(uint32_t num, char* str) {
    const char hex_digits[] = "0123456789ABCDEF";
    str[0] = '0';
    str[1] = 'x';
    
    for(int i = 0; i < 8; i++) {
        str[9 - i] = hex_digits[num & 0xF];
        num >>= 4;
    }
    str[10] = '\0';
}

/* Display memory information for 5 seconds */
void display_memory_info(void) {
    memory_info_t mem;
    
    /* Get ACTUAL memory info from PMM */
    mem.total_mb = pmm_get_total_memory() / 1048576;  /* Convert to MB */
    mem.used_mb = pmm_get_used_memory() / 1048576;
    mem.free_kb = pmm_get_free_memory() / 1024;  /* Convert to KB */
    
    screen_clear();
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    screen_write("\n\n");
    screen_write("========================================\n");
    screen_write("      MEMORY INFORMATION\n");
    screen_write("========================================\n");
    
    char buffer[64];
    
    /* Format numbers */
    screen_write("Total Memory...........");
    int_to_str(mem.total_mb, buffer);
    screen_write(buffer);
    screen_write(" MB (");
    int_to_str(pmm_get_total_pages(), buffer);
    screen_write(buffer);
    screen_write(" pages)\n");
    
    screen_write("Used Memory............");
    int_to_str(mem.used_mb, buffer);
    screen_write(buffer);
    screen_write(" MB (");
    int_to_str(pmm_get_total_pages() - pmm_get_free_pages(), buffer);
    screen_write(buffer);
    screen_write(" pages)\n");
    
    screen_write("Free Memory............");
    int_to_str(mem.free_kb / 1024, buffer);
    screen_write(buffer);
    screen_write(" MB (");
    int_to_str(mem.free_kb, buffer);
    screen_write(buffer);
    screen_write(" KB / ");
    int_to_str(pmm_get_free_pages(), buffer);
    screen_write(buffer);
    screen_write(" pages)\n");
    
    screen_write("Memory Usage...........");
    uint32_t percent = (mem.used_mb * 100) / mem.total_mb;
    int_to_str(percent, buffer);
    screen_write(buffer);
    screen_write("%%\n");
    
    screen_write("========================================\n");
    
    /* Display kernel information */
    screen_write("\nKernel Information:\n");
    uint32_t kernel_start = (uint32_t)&_kernel_start;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    uint32_t kernel_size = kernel_end - kernel_start;
    
    screen_write("Kernel Start: 0x");
    int_to_hex(kernel_start, buffer);
    screen_write(buffer);
    screen_write("\n");
    
    screen_write("Kernel End:   0x");
    int_to_hex(kernel_end, buffer);
    screen_write(buffer);
    screen_write("\n");
    
    screen_write("Kernel Size:  ");
    int_to_str(kernel_size / 1024, buffer);
    screen_write(buffer);
    if(kernel_size % 1024 != 0) {
        screen_write(".");
        int_to_str((kernel_size % 1024) * 100 / 1024, buffer);
        screen_write(buffer);
    }
    screen_write(" KB (");
    int_to_str(kernel_size, buffer);
    screen_write(buffer);
    screen_write(" bytes)\n");
    
    screen_write("========================================\n");
    
    /* Wait 5 seconds */
    for(volatile int i = 0; i < 500000000; i++);
}

/* Display welcome screen */
void display_welcome(void) {
    screen_clear();
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    screen_write("\n\n");
    screen_write("========================================\n");
    screen_write("         welcome to InstaChromeOS\n");
    screen_write("========================================\n");
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
            
            /* Print a newline before command output to move it one line down */
            screen_write("\n");
            
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
            /* Keyboard layout switching command */
            else if(strncmp(command, "KeySifh ", 8) == 0) {
                char* layout_cmd = command + 8;
                
                if(strcmp(layout_cmd, "sv") == 0) {
                    keyboard_set_layout(LAYOUT_SWEDISH);
                    screen_write("Keyboard layout switched to Swedish\n");
                    screen_write("Now you can type å, ä, ö!\n");
                }
                else if(strcmp(layout_cmd, "us eng") == 0) {
                    keyboard_set_layout(LAYOUT_US_ENGLISH);
                    screen_write("Keyboard layout switched to US English\n");
                }
                else {
                    screen_write("Usage: KeySifh <layout>\n");
                    screen_write("Available layouts: sv, us eng\n");
                    screen_write("  sv     - Swedish layout (å, ä, ö)\n");
                    screen_write("  us eng - US English layout\n");
                }
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
                screen_write("  KeySifh <layout> - Switch keyboard layout\n");
                screen_write("                     sv = Swedish, us eng = US English\n");
                screen_write("  help             - Show this help\n");
                screen_write("\n");
                screen_write("Current layout: ");
                screen_write(keyboard_get_layout_name());
                screen_write("\n\n");
            }
            else if(cmd_index > 0) {
                screen_write("Command not found. Type 'help' for available commands.\n");
            }
            
            /* Print new prompt */
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