# Kernel Makefile for InstaChromeOS - VGA Mode 13h version

CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Iinclude
LDFLAGS = -T linker.ld -ffreestanding -nostdlib -lgcc

# Source files - VGA används, INTE fb
C_SOURCES = kernel.c \
            drivers/screen.c \
            drivers/keyboard.c \
            drivers/vga.c \
            drivers/font.c \
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
.PHONY: all clean size

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

# Show kernel size
size: kernel.elf
	@echo "Kernel size:"
	@stat -c %s kernel.elf 2>/dev/null || echo "Kernel not built"

# Show all object files sizes
sizes: $(OBJECTS)
	@echo "Object file sizes:"
	@for obj in $(OBJECTS); do \
		echo -n "  $$obj: "; \
		stat -c %s $$obj 2>/dev/null || echo "0"; \
	done

# Disassemble kernel (for debugging)
disassemble: kernel.elf
	i686-elf-objdump -d kernel.elf > kernel.dis

# Print compiler info
info:
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Linker: $(LD)"
	@echo "Linker flags: $(LDFLAGS)"
	@echo ""
	@echo "Source files:"
	@for src in $(C_SOURCES); do \
		echo "  $$src"; \
	done
	@echo ""
	@echo "Assembly files:"
	@for asm in $(ASM_SOURCES); do \
		echo "  $$asm"; \
	done