#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>

/* BIOS function declarations */
void display_bios_screen(void);
void enter_bios(void);
int check_bios_key(void);
void bios_menu(void);
int is_in_bios(void);

/* BIOS key combinations */
#define BIOS_KEY_COMBINATION 0x01  /* Right Shift + F12 */
#define SETUP_KEY_COMBINATION 0x02  /* Right Shift + F10 */

#endif