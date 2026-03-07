#!/bin/bash

# InstaChromeOS Build Script
# This script builds the entire operating system and creates a bootable ISO

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="InstaChromeOS"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KERNEL_DIR="$PROJECT_ROOT/kernel"
BUILD_DIR="$PROJECT_ROOT/build"
ISO_DIR="$PROJECT_ROOT/iso"
ISO_NAME="$PROJECT_ROOT/$PROJECT_NAME.iso"

# Print banner
print_banner() {
    echo -e "${BLUE}"
    echo "=========================================="
    echo "     InstaChromeOS Build System v1.0"
    echo "=========================================="
    echo -e "${NC}"
}

# Check required tools
check_tools() {
    echo -e "${YELLOW}[1/8] Checking required tools...${NC}"
    
    REQUIRED_TOOLS=("i686-elf-gcc" "i686-elf-as" "i686-elf-ld" "grub-mkrescue" "xorriso" "qemu-system-i386" "make")
    MISSING_TOOLS=()
    
    for tool in "${REQUIRED_TOOLS[@]}"; do
        if ! command -v $tool &> /dev/null; then
            MISSING_TOOLS+=($tool)
        fi
    done
    
    if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
        echo -e "${RED}Error: Missing required tools:${NC}"
        printf '%s\n' "${MISSING_TOOLS[@]}"
        echo -e "${YELLOW}Please install the missing tools and try again.${NC}"
        echo "On Ubuntu/Debian: sudo apt-get install build-essential nasm qemu-system-x86 grub2-common xorriso"
        echo "You'll also need an i686-elf cross-compiler toolchain."
        exit 1
    fi
    
    echo -e "${GREEN}✓ All required tools found${NC}"
}

# Clean build directory
clean_build() {
    echo -e "${YELLOW}[2/8] Cleaning previous builds...${NC}"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo "  Removed build directory"
    fi
    
    if [ -d "$ISO_DIR" ]; then
        rm -rf "$ISO_DIR"
        echo "  Removed ISO directory"
    fi
    
    if [ -f "$ISO_NAME" ]; then
        rm -f "$ISO_NAME"
        echo "  Removed old ISO file"
    fi
    
    if [ -f "$KERNEL_DIR/kernel.elf" ]; then
        rm -f "$KERNEL_DIR/kernel.elf"
        echo "  Removed old kernel"
    fi
    
    echo -e "${GREEN}✓ Clean completed${NC}"
}

# Build kernel
build_kernel() {
    echo -e "${YELLOW}[3/8] Building kernel...${NC}"
    
    cd "$KERNEL_DIR"
    
    # Clean kernel objects
    find . -name "*.o" -type f -delete
    
    # Build kernel using make
    if make; then
        echo -e "${GREEN}✓ Kernel built successfully${NC}"
    else
        echo -e "${RED}✗ Kernel build failed${NC}"
        exit 1
    fi
    
    cd "$PROJECT_ROOT"
}

# Verify kernel
verify_kernel() {
    echo -e "${YELLOW}[4/8] Verifying kernel...${NC}"
    
    KERNEL_FILE="$KERNEL_DIR/kernel.elf"
    
    if [ ! -f "$KERNEL_FILE" ]; then
        echo -e "${RED}✗ Kernel file not found${NC}"
        exit 1
    fi
    
    # Check kernel size
    KERNEL_SIZE=$(stat -c%s "$KERNEL_FILE")
    echo "  Kernel size: $KERNEL_SIZE bytes"
    
    if [ $KERNEL_SIZE -eq 0 ]; then
        echo -e "${RED}✗ Kernel file is empty${NC}"
        exit 1
    fi
    
    # Check if it's a valid ELF file
    if file "$KERNEL_FILE" | grep -q "ELF"; then
        echo -e "${GREEN}✓ Kernel is valid ELF file${NC}"
    else
        echo -e "${RED}✗ Kernel is not a valid ELF file${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Kernel verification passed${NC}"
}

# Create directory structure
create_dirs() {
    echo -e "${YELLOW}[5/8] Creating ISO directory structure...${NC}"
    
    mkdir -p "$ISO_DIR/boot/grub"
    mkdir -p "$ISO_DIR/InstaChromeOS/system/int_handler"
    mkdir -p "$ISO_DIR/InstaChromeOS/personal"
    mkdir -p "$ISO_DIR/InstaChromeOS/space"
    mkdir -p "$ISO_DIR/InstaChromeOS/tmp"
    
    echo "  Created ISO directory structure"
    echo -e "${GREEN}✓ Directory structure created${NC}"
}

# Copy files to ISO
copy_files() {
    echo -e "${YELLOW}[6/8] Copying files to ISO...${NC}"
    
    # Copy kernel
    cp "$KERNEL_DIR/kernel.elf" "$ISO_DIR/boot/"
    echo "  Copied kernel.elf"
    
    # Copy GRUB configuration
    if [ -f "$PROJECT_ROOT/boot/grub/grub.cfg" ]; then
        cp "$PROJECT_ROOT/boot/grub/grub.cfg" "$ISO_DIR/boot/grub/"
        echo "  Copied grub.cfg"
    else
        # Create default GRUB config
        cat > "$ISO_DIR/boot/grub/grub.cfg" << EOF
set timeout=3
set default=0

menuentry "InstaChromeOS" {
    multiboot2 /boot/kernel.elf
    boot
}
EOF
        echo "  Created default grub.cfg"
    fi
    
    # Copy system files
    if [ -f "$PROJECT_ROOT/system/int_handler/int.ih" ]; then
        cp "$PROJECT_ROOT/system/int_handler/int.ih" "$ISO_DIR/InstaChromeOS/system/int_handler/"
        echo "  Copied int.ih"
    else
        # Create default int.ih
        cat > "$ISO_DIR/InstaChromeOS/system/int_handler/int.ih" << EOF
// Interrupt Handler Configuration
[INTERRUPTS]
IRQ0=Timer
IRQ1=Keyboard
EOF
        echo "  Created default int.ih"
    fi
    
    if [ -f "$PROJECT_ROOT/system/OSsys.os" ]; then
        cp "$PROJECT_ROOT/system/OSsys.os" "$ISO_DIR/InstaChromeOS/system/"
        echo "  Copied OSsys.os"
    fi
    
    if [ -f "$PROJECT_ROOT/system/Linker.hmk" ]; then
        cp "$PROJECT_ROOT/system/Linker.hmk" "$ISO_DIR/InstaChromeOS/system/"
        echo "  Copied Linker.hmk"
    fi
    
    if [ -f "$PROJECT_ROOT/system/instachrome.jl" ]; then
        cp "$PROJECT_ROOT/system/instachrome.jl" "$ISO_DIR/InstaChromeOS/system/"
        echo "  Copied instachrome.jl"
    fi
    
    # Copy README files
    cat > "$ISO_DIR/InstaChromeOS/personal/README.txt" << EOF
Personal Directory
==================
This directory is for personal files.
EOF
    echo "  Created personal README"
    
    cat > "$ISO_DIR/InstaChromeOS/space/README.txt" << EOF
Space Directory
===============
This directory is for temporary workspace.
EOF
    echo "  Created space README"
    
    cat > "$ISO_DIR/InstaChromeOS/tmp/README.txt" << EOF
Temporary Directory
===================
Temporary files are stored here.
EOF
    echo "  Created tmp README"
    
    echo -e "${GREEN}✓ Files copied successfully${NC}"
}

# Create ISO
create_iso() {
    echo -e "${YELLOW}[7/8] Creating bootable ISO...${NC}"
    
    # Create ISO using grub-mkrescue
    if grub-mkrescue -o "$ISO_NAME" "$ISO_DIR" 2>/dev/null; then
        echo -e "${GREEN}✓ ISO created successfully: $ISO_NAME${NC}"
    else
        # Fallback method using xorriso directly
        echo "  Grub-mkrescue failed, trying xorriso fallback..."
        
        xorriso -as mkisofs \
            -b boot/grub/i386-pc/eltorito.img \
            -no-emul-boot \
            -boot-load-size 4 \
            -boot-info-table \
            --grub2-boot-info \
            --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img \
            -o "$ISO_NAME" \
            "$ISO_DIR" 2>/dev/null
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ ISO created successfully (xorriso method): $ISO_NAME${NC}"
        else
            echo -e "${RED}✗ Failed to create ISO${NC}"
            exit 1
        fi
    fi
    
    # Get ISO size
    ISO_SIZE=$(du -h "$ISO_NAME" | cut -f1)
    echo "  ISO size: $ISO_SIZE"
}

# Test ISO with QEMU
test_iso() {
    echo -e "${YELLOW}[8/8] Testing ISO with QEMU...${NC}"
    echo -e "${BLUE}Starting QEMU in 3 seconds...${NC}"
    echo -e "${YELLOW}(Press Ctrl+A then X to exit QEMU)${NC}"
    sleep 3
    
    qemu-system-i386 -cdrom "$ISO_NAME" -m 128M -vga std
    
    echo -e "${GREEN}✓ QEMU test completed${NC}"
}

# Print build summary
print_summary() {
    echo -e "${BLUE}"
    echo "=========================================="
    echo "        Build Complete!"
    echo "=========================================="
    echo -e "${NC}"
    echo -e "Project: ${GREEN}$PROJECT_NAME${NC}"
    echo -e "ISO File: ${GREEN}$ISO_NAME${NC}"
    echo -e "ISO Size: ${YELLOW}$(du -h "$ISO_NAME" | cut -f1)${NC}"
    echo
    echo -e "${BLUE}You can now:${NC}"
    echo "  1. Run in QEMU:    qemu-system-i386 -cdrom $ISO_NAME"
    echo "  2. Burn to USB:     sudo dd if=$ISO_NAME of=/dev/sdb bs=4M status=progress"
    echo "  3. Burn to CD:      growisofs -dvd-compat -Z /dev/sr0=$ISO_NAME"
    echo
    echo -e "${GREEN}Thank you for using InstaChromeOS!${NC}"
}

# Show help
show_help() {
    echo "Usage: ./build.sh [OPTION]"
    echo "Build InstaChromeOS operating system"
    echo
    echo "Options:"
    echo "  all       - Full build (clean, kernel, iso, test)"
    echo "  kernel    - Build only the kernel"
    echo "  iso       - Create ISO from existing kernel"
    echo "  clean     - Clean build directories"
    echo "  test      - Test ISO with QEMU"
    echo "  help      - Show this help message"
    echo
    echo "Examples:"
    echo "  ./build.sh        - Full build"
    echo "  ./build.sh kernel - Build kernel only"
    echo "  ./build.sh test   - Test existing ISO"
}

# Main build function
main() {
    print_banner
    
    case "${1:-all}" in
        "all")
            check_tools
            clean_build
            build_kernel
            verify_kernel
            create_dirs
            copy_files
            create_iso
            print_summary
            ;;
        "kernel")
            check_tools
            build_kernel
            verify_kernel
            echo -e "${GREEN}Kernel build completed!${NC}"
            ;;
        "iso")
            check_tools
            verify_kernel
            create_dirs
            copy_files
            create_iso
            echo -e "${GREEN}ISO creation completed!${NC}"
            ;;
        "clean")
            clean_build
            ;;
        "test")
            if [ -f "$ISO_NAME" ]; then
                test_iso
            else
                echo -e "${RED}No ISO found. Run full build first.${NC}"
                exit 1
            fi
            ;;
        "help")
            show_help
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
}

# Make script executable
# Run main function with all arguments
main "$@"