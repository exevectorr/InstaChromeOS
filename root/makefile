# InstaChromeOS Master Makefile

# Directories
KERNEL_DIR = InstaChromeOS/kernel
BOOT_DIR = InstaChromeOS/boot
ISO_DIR = isodir
ISO_BOOT_DIR = $(ISO_DIR)/boot
ISO_GRUB_DIR = $(ISO_BOOT_DIR)/grub

# Tools
MKRESCUE = grub-mkrescue
QEMU = qemu-system-i386

# Targets
.PHONY: all kernel iso clean run

all: kernel iso

# Build kernel
kernel:
	$(MAKE) -C $(KERNEL_DIR)

# Create ISO
iso: kernel
	mkdir -p $(ISO_GRUB_DIR)
	cp $(KERNEL_DIR)/kernel.elf $(ISO_BOOT_DIR)/
	cp InstaChromeOS/boot/grub/grub.cfg $(ISO_GRUB_DIR)/
	$(MKRESCUE) -o InstaChromeOS.iso $(ISO_DIR)

# Clean everything
clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	rm -rf $(ISO_DIR)
	rm -f InstaChromeOS.iso

# Run in QEMU
run: iso
	$(QEMU) -cdrom InstaChromeOS.iso

# Debug with QEMU and GDB
debug: iso
	$(QEMU) -cdrom InstaChromeOS.iso -s -S &
	gdb $(KERNEL_DIR)/kernel.elf -ex "target remote localhost:1234"