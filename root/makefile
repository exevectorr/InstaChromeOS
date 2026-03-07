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
.PHONY: all kernel iso clean run debug size

all: kernel iso

# Build kernel
kernel:
	$(MAKE) -C $(KERNEL_DIR)

# Show kernel size
size: kernel
	$(MAKE) -C $(KERNEL_DIR) size

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

# Run in QEMU with 128MB default
run: iso
	$(QEMU) -cdrom InstaChromeOS.iso -m 128M

# Run with specific memory size
run256: iso
	$(QEMU) -cdrom InstaChromeOS.iso -m 256M

run512: iso
	$(QEMU) -cdrom InstaChromeOS.iso -m 512M

# Debug with QEMU and GDB
debug: iso
	$(QEMU) -cdrom InstaChromeOS.iso -m 128M -s -S &
	@echo "Waiting for QEMU to start..."
	@sleep 2
	gdb $(KERNEL_DIR)/kernel.elf -ex "target remote localhost:1234"

# Quick test with serial output
serial: iso
	$(QEMU) -cdrom InstaChromeOS.iso -m 128M -serial stdio

# Install to USB (be careful with this!)
install: iso
	@echo "WARNING: This will overwrite /dev/sdb!"
	@read -p "Press Enter to continue or Ctrl+C to abort"
	sudo dd if=InstaChromeOS.iso of=/dev/sdb bs=4M status=progress && sync