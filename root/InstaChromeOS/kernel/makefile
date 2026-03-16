# Kernel Makefile for InstaChromeOS

CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Iinclude
LDFLAGS = -T linker.ld -ffreestanding -nostdlib -lgcc

# Source files
C_SOURCES = kernel.c \
            drivers/screen.c \
            drivers/keyboard.c \
            mm/pmm.c \
            fs/fat32.c \
            lib/string.c \
            bios/bios.c \
            power/shutdown.c

ASM_SOURCES = boot/boot.s

# Object files
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Targets
.PHONY: all clean

all: kernel.elf

# Assembly to object
%.o: %.s
	$(AS) $< -o $@

# C to object
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Link kernel
kernel.elf: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Clean
clean:
	rm -f $(OBJECTS) kernel.elf