#include "kernel.h"
#include "boot/multiboot.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "mm/pmm.h"
#include "fs/fat32.h"
#include "lib/string.h"
#include "bios/bios.h"
#include "power/shutdown.h"
#include "drivers/vga.h"
#include "drivers/font.h"

/* External symbols from linker script */
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

/* Global variables for graphics shell */
static int g_cursor_col = 0;
static int g_cursor_row = 2;
static int g_prompt_row = 2;
static char g_current_dir[256] = "/";

/* VGA info */
static int vga_width = 320;
static int vga_height = 200;

/* Function declarations */
void graphics_write(const char* str);
void graphics_write_color(const char* str, uint8_t color);
void graphics_clear_line(int row);
void uint32_to_str(uint32_t num, char* str);
void display_bios_screen_graphics(void);
void display_memory_info_graphics(void);
void display_welcome_graphics(void);
void start_shell_graphics(void);

/* Konvertera kolumn/rad till pixelkoordinater */
int col_to_x(int col) { return col * 8; }
int row_to_y(int row) { return row * 8; }

/* Kernel main function */
void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    (void)addr;
    
    /* Sätt VGA Mode 13h (320x200, 256 färger) */
    vga_set_mode_13h();
    
    /* Sätt blå bakgrund */
    vga_clear_screen(1);
    
    /* Initiera övriga subsystem */
    init_memory();
    init_keyboard();
    init_fs();
    
    /* Visa BIOS-skärm */
    display_bios_screen_graphics();
    
    /* Visa minnesinfo */
    display_memory_info_graphics();
    
    /* Visa välkomstskärm och starta shell */
    display_welcome_graphics();
    start_shell_graphics();
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

void init_memory(void) {
    pmm_init();
}

void uint32_to_str(uint32_t num, char* str) {
    int i = 0;
    if(num == 0) {
        str[i++] = '0';
    } else {
        uint32_t n = num;
        while(n > 0) {
            str[i++] = '0' + (n % 10);
            n /= 10;
        }
    }
    str[i] = '\0';
    
    for(int j = 0; j < i/2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

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

/* Rita ett tecken med VGA-färg */
void draw_char_vga(int x, int y, char c, uint8_t color) {
    unsigned char uc = (unsigned char)c;
    uint8_t* glyph;
    
    if(uc >= 128 && uc <= 143) {
        glyph = font_8x8[uc];
    }
    else if(uc >= 32 && uc <= 126) {
        glyph = font_8x8[uc - 32];
    }
    else {
        glyph = font_8x8[0];
    }
    
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            if(glyph[row] & (1 << (7 - col))) {
                vga_put_pixel(x + col, y + row, color);
            }
        }
    }
}

/* Rita en sträng med VGA-färg */
void draw_string_vga(int x, int y, const char* str, uint8_t color) {
    int start_x = x;
    while(*str) {
        if(*str == '\n') {
            x = start_x;
            y += 8;
        } else {
            draw_char_vga(x, y, *str, color);
            x += 8;
        }
        str++;
    }
}

/* Graphics version of BIOS screen */
void display_bios_screen_graphics(void) {
    vga_clear_screen(1);
    
    draw_string_vga(col_to_x(0), row_to_y(1), "========================================", 7);
    draw_string_vga(col_to_x(0), row_to_y(2), "          InstaChromeOS BIOS", 15);
    draw_string_vga(col_to_x(0), row_to_y(3), "========================================", 7);
    draw_string_vga(col_to_x(0), row_to_y(5), "Version: 1.2.0", 7);
    draw_string_vga(col_to_x(0), row_to_y(6), "Build: March 2026", 7);
    draw_string_vga(col_to_x(0), row_to_y(7), "CPU: i686", 7);
    
    char buf[16];
    draw_string_vga(col_to_x(0), row_to_y(8), "Memory: ", 7);
    uint32_to_str(pmm_get_total_memory() / 1048576, buf);
    draw_string_vga(col_to_x(9), row_to_y(8), buf, 10);
    draw_string_vga(col_to_x(11), row_to_y(8), "MB", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(10), "Press F2 for setup, ESC to continue", 14);
    
    for(volatile int i = 0; i < 250000000; i++);
}

/* Graphics version of memory info */
void display_memory_info_graphics(void) {
    uint32_t total_mb = pmm_get_total_memory() / 1048576;
    uint32_t used_mb = pmm_get_used_memory() / 1048576;
    uint32_t free_kb = pmm_get_free_memory() / 1024;
    
    vga_clear_screen(1);
    
    char buffer[64];
    
    draw_string_vga(col_to_x(0), row_to_y(1), "========================================", 7);
    draw_string_vga(col_to_x(0), row_to_y(2), "         MEMORY INFORMATION", 15);
    draw_string_vga(col_to_x(0), row_to_y(3), "========================================", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(5), "Total Memory:", 7);
    uint32_to_str(total_mb, buffer);
    draw_string_vga(col_to_x(15), row_to_y(5), buffer, 10);
    draw_string_vga(col_to_x(18), row_to_y(5), "MB", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(6), "Used Memory:", 7);
    uint32_to_str(used_mb, buffer);
    draw_string_vga(col_to_x(15), row_to_y(6), buffer, 12);
    draw_string_vga(col_to_x(18), row_to_y(6), "MB", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(7), "Free Memory:", 7);
    uint32_to_str(free_kb / 1024, buffer);
    draw_string_vga(col_to_x(15), row_to_y(7), buffer, 10);
    draw_string_vga(col_to_x(18), row_to_y(7), "MB", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(8), "Free (KB):", 7);
    uint32_to_str(free_kb, buffer);
    draw_string_vga(col_to_x(15), row_to_y(8), buffer, 14);
    draw_string_vga(col_to_x(23), row_to_y(8), "KB", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(9), "Memory Usage:", 7);
    uint32_t percent = (used_mb * 100) / total_mb;
    uint32_to_str(percent, buffer);
    draw_string_vga(col_to_x(15), row_to_y(9), buffer, 14);
    draw_string_vga(col_to_x(17), row_to_y(9), "%", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(11), "========================================", 7);
    
    draw_string_vga(col_to_x(0), row_to_y(13), "Kernel Information:", 15);
    
    uint32_t kernel_start = (uint32_t)&_kernel_start;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    uint32_t kernel_size = kernel_end - kernel_start;
    
    draw_string_vga(col_to_x(0), row_to_y(14), "Kernel Size:", 7);
    uint32_to_str(kernel_size / 1024, buffer);
    draw_string_vga(col_to_x(15), row_to_y(14), buffer, 10);
    draw_string_vga(col_to_x(18), row_to_y(14), "KB", 7);
    
    for(volatile int i = 0; i < 500000000; i++);
}

/* Graphics version of welcome screen */
void display_welcome_graphics(void) {
    vga_clear_screen(1);
    
    draw_string_vga(col_to_x(0), row_to_y(1), "========================================", 7);
    draw_string_vga(col_to_x(0), row_to_y(2), "         welcome to InstaChromeOS", 15);
    draw_string_vga(col_to_x(0), row_to_y(3), "========================================", 7);
    draw_string_vga(col_to_x(0), row_to_y(5), "                  Powerd by InstaChrome", 10);
    draw_string_vga(col_to_x(0), row_to_y(7), "root> ", 15);
    
    g_cursor_col = 7;
    g_cursor_row = 7;
    g_prompt_row = 7;
}

/* Helper function to write text in graphics mode */
void graphics_write(const char* str) {
    while(*str) {
        if(*str == '\n') {
            g_cursor_col = 0;
            g_cursor_row++;
        } else {
            draw_char_vga(col_to_x(g_cursor_col), row_to_y(g_cursor_row), *str, 7);
            g_cursor_col++;
        }
        str++;
    }
}

/* Helper function to write colored text */
void graphics_write_color(const char* str, uint8_t color) {
    while(*str) {
        if(*str == '\n') {
            g_cursor_col = 0;
            g_cursor_row++;
        } else {
            draw_char_vga(col_to_x(g_cursor_col), row_to_y(g_cursor_row), *str, color);
            g_cursor_col++;
        }
        str++;
    }
}

/* Helper function to clear current line */
void graphics_clear_line(int row) {
    int y = row_to_y(row);
    for(int x = 0; x < vga_width; x++) {
        for(int dy = 0; dy < 8; dy++) {
            vga_put_pixel(x, y + dy, 1);
        }
    }
}

/* Graphics version of shell */
void start_shell_graphics(void) {
    char command[41];
    int cmd_index = 0;
    char num_buf[16];
    int bytes_read;
    
    while(1) {
        int c = keyboard_getchar();
        
        if(c == '\n') {
            command[cmd_index] = '\0';
            
            graphics_clear_line(g_cursor_row);
            g_cursor_row++;
            g_cursor_col = 0;
            
            if(strcmp(command, "dir") == 0) {
                graphics_write_color("Root/\n", 15);
                graphics_write_color("+-------InstaChromeOS\n", 7);
                graphics_write_color("+                      +---------system/\n", 7);
                graphics_write_color("+                      +                     +----int_handler/\n", 7);
                graphics_write_color("+                      +                     +                 +----int.ih\n", 7);
                graphics_write_color("+                      +                     +---- OSsys.os\n", 7);
                graphics_write_color("+                      +                     +---- Linker.hmk\n", 7);
                graphics_write_color("+                      +                     +---- instachrome.jl\n", 7);
                graphics_write_color("+                      +------ personal/\n", 7);
                graphics_write_color("+                      +------ space/\n", 7);
                graphics_write_color("+                      +------ tmp/\n", 7);
            }
            else if(strcmp(command, "lst") == 0) {
                graphics_write_color("Directory contents of ", 15);
                graphics_write(g_current_dir);
                graphics_write("\n");
                graphics_write_color("----------------------------------------\n", 7);
                graphics_write_color("[DIR]  system/\n", 7);
                graphics_write_color("[DIR]  personal/\n", 7);
                graphics_write_color("[DIR]  space/\n", 7);
                graphics_write_color("[DIR]  tmp/\n", 7);
                graphics_write_color("----------------------------------------\n", 7);
            }
            else if(strncmp(command, "cd ", 3) == 0) {
                char* path = command + 3;
                fs_change_dir(path);
                fs_get_path(g_current_dir, sizeof(g_current_dir));
                graphics_write_color("Changed to directory: ", 10);
                graphics_write(g_current_dir);
                graphics_write("\n");
            }
            else if(strncmp(command, "tayn ", 5) == 0) {
                char* filename = command + 5;
                fs_create_file(filename);
                graphics_write_color("File '", 10);
                graphics_write(filename);
                graphics_write_color("' created\n", 10);
            }
            else if(strncmp(command, "mkdir ", 6) == 0) {
                char* dirname = command + 6;
                fs_create_dir(dirname);
                graphics_write_color("Directory '", 10);
                graphics_write(dirname);
                graphics_write_color("' created\n", 10);
            }
            else if(strncmp(command, "printf * ", 9) == 0) {
                graphics_write_color(command + 9, 7);
                graphics_write("\n");
            }
            else if(strcmp(command, "clear") == 0) {
                vga_clear_screen(1);
                g_cursor_col = 7;
                g_cursor_row = 7;
                g_prompt_row = 7;
                draw_string_vga(col_to_x(0), row_to_y(g_prompt_row), "root> ", 15);
            }
            else if(strcmp(command, "shutdown") == 0) {
                graphics_write_color("Shutting down...\n", 12);
                shutdown_system();
            }
            else if(strcmp(command, "reboot") == 0) {
                graphics_write_color("Rebooting...\n", 14);
                reboot_system();
            }
            else if(strncmp(command, "run -v ", 7) == 0) {
                char* filename = command + 7;
                char file_buffer[4096];
                bytes_read = fs_read_file(filename, file_buffer, 4096);
                
                if(bytes_read > 0) {
                    graphics_write_color("--- Content of ", 15);
                    graphics_write(filename);
                    uint32_to_str(bytes_read, num_buf);
                    graphics_write_color(" (", 7);
                    graphics_write(num_buf);
                    graphics_write_color(" bytes) ---\n", 7);
                    graphics_write(file_buffer);
                    graphics_write("\n");
                    graphics_write_color("--- End of file ---\n", 7);
                } else {
                    graphics_write_color("File not found: ", 12);
                    graphics_write(filename);
                    graphics_write("\n");
                }
            }
            else if(strncmp(command, "load -s ", 8) == 0) {
                char* filename = command + 8;
                char existing_content[4096] = {0};
                bytes_read = fs_read_file(filename, existing_content, 4096);
                
                graphics_write_color("Editing file: ", 15);
                graphics_write(filename);
                graphics_write("\n");
                
                if(bytes_read > 0) {
                    graphics_write_color("Current content:\n", 7);
                    graphics_write_color("------------------------------------------------\n", 7);
                    graphics_write(existing_content);
                    if(existing_content[0] != '\0' && existing_content[bytes_read-1] != '\n') {
                        graphics_write("\n");
                    }
                    graphics_write_color("------------------------------------------------\n", 7);
                }
                
                graphics_write_color("Enter new content (F2 to save, ESC to cancel):\n", 15);
                
                int edit_index = 0;
                int editing = 1;
                char edit_buffer[4096] = {0};
                int edit_col = 0;
                int edit_row = g_cursor_row;
                
                if(bytes_read > 0) {
                    for(int i = 0; i < bytes_read; i++) {
                        edit_buffer[edit_index++] = existing_content[i];
                        draw_char_vga(col_to_x(edit_col), row_to_y(edit_row), existing_content[i], 7);
                        edit_col++;
                        if(edit_col >= 40) {
                            edit_col = 0;
                            edit_row++;
                        }
                    }
                }
                
                while(editing) {
                    int key = keyboard_getchar();
                    
                    if(key == KEY_ESC) {
                        graphics_write_color("\nEdit cancelled.\n", 12);
                        editing = 0;
                    }
                    else if(key == KEY_F2) {
                        edit_buffer[edit_index] = '\0';
                        
                        if(fs_write_file(filename, edit_buffer) == 0) {
                            graphics_write_color("\nFile saved successfully (", 10);
                            uint32_to_str(edit_index, num_buf);
                            graphics_write(num_buf);
                            graphics_write_color(" bytes).\n", 10);
                        } else {
                            graphics_write_color("\nError saving file.\n", 12);
                        }
                        editing = 0;
                        g_cursor_row = edit_row + 2;
                    }
                    else if(key == KEY_ENTER) {
                        if(edit_index < 4095) {
                            edit_buffer[edit_index++] = '\n';
                            edit_col = 0;
                            edit_row++;
                        }
                    }
                    else if(key == KEY_BACKSPACE && edit_index > 0) {
                        edit_index--;
                        edit_col--;
                        if(edit_col < 0) {
                            edit_col = 39;
                            edit_row--;
                        }
                        int x = col_to_x(edit_col);
                        int y = row_to_y(edit_row);
                        for(int i = 0; i < 8; i++) {
                            for(int j = 0; j < 8; j++) {
                                vga_put_pixel(x + i, y + j, 1);
                            }
                        }
                    }
                    else if(key >= 32 && key <= 126 && edit_index < 4095) {
                        edit_buffer[edit_index++] = key;
                        draw_char_vga(col_to_x(edit_col), row_to_y(edit_row), key, 7);
                        edit_col++;
                        if(edit_col >= 40) {
                            edit_col = 0;
                            edit_row++;
                        }
                    }
                }
            }
            else if(strncmp(command, "KeySifh ", 8) == 0) {
                char* layout_cmd = command + 8;
                
                if(strcmp(layout_cmd, "sv") == 0) {
                    keyboard_set_layout(LAYOUT_SWEDISH);
                    graphics_write_color("Swedish layout activated\n", 10);
                }
                else if(strcmp(layout_cmd, "us eng") == 0) {
                    keyboard_set_layout(LAYOUT_US_ENGLISH);
                    graphics_write_color("US English layout activated\n", 10);
                }
                else {
                    graphics_write_color("Usage: KeySifh <sv|us eng>\n", 12);
                }
            }
            else if(strcmp(command, "help") == 0) {
                graphics_write_color("========================================\n", 7);
                graphics_write_color("        Available Commands\n", 15);
                graphics_write_color("========================================\n", 7);
                graphics_write("\n");
                graphics_write_color("  dir              - Display full filesystem tree\n", 7);
                graphics_write_color("  lst              - List current directory contents\n", 7);
                graphics_write_color("  cd <dir>         - Change directory\n", 7);
                graphics_write_color("  tayn <file>      - Create a new empty file\n", 7);
                graphics_write_color("  mkdir <dir>      - Create a new directory\n", 7);
                graphics_write_color("  run -v <file>    - View file content (read-only)\n", 7);
                graphics_write_color("  load -s <file>   - Edit file content (F2 to save)\n", 7);
                graphics_write_color("  printf * <text>  - Print text to screen\n", 7);
                graphics_write_color("  clear            - Clear the screen\n", 7);
                graphics_write_color("  shutdown         - Shut down the system\n", 7);
                graphics_write_color("  reboot           - Reboot the system\n", 7);
                graphics_write_color("  KeySifh <layout> - Switch keyboard layout\n", 7);
                graphics_write_color("  help             - Show this help screen\n", 7);
                graphics_write("\n");
                graphics_write_color("Keyboard Layouts:\n", 15);
                graphics_write_color("  sv               - Swedish layout (å = [, ö = ;, ä = ')\n", 7);
                graphics_write_color("  us eng           - US English layout\n", 7);
                graphics_write("\n");
                graphics_write_color("Current layout: ", 15);
                graphics_write(keyboard_get_layout_name());
                graphics_write("\n");
            }
            else if(cmd_index > 0) {
                graphics_write_color("Command not found. Type 'help'\n", 12);
            }
            
            g_cursor_row++;
            g_prompt_row = g_cursor_row;
            draw_string_vga(col_to_x(0), row_to_y(g_prompt_row), "root> ", 15);
            g_cursor_col = 7;
            cmd_index = 0;
            
            if(g_cursor_row >= 22) {
                vga_clear_screen(1);
                g_cursor_col = 7;
                g_cursor_row = 7;
                g_prompt_row = 7;
                draw_string_vga(col_to_x(0), row_to_y(g_prompt_row), "root> ", 15);
            }
        }
        else if(c == KEY_BACKSPACE && cmd_index > 0) {
            cmd_index--;
            g_cursor_col--;
            int x = col_to_x(g_cursor_col);
            int y = row_to_y(g_cursor_row);
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    vga_put_pixel(x + i, y + j, 1);
                }
            }
        }
        else if(c >= 32 && c <= 126 && cmd_index < 40) {
            command[cmd_index++] = c;
            draw_char_vga(col_to_x(g_cursor_col), row_to_y(g_cursor_row), c, 15);
            g_cursor_col++;
        }
    }
}