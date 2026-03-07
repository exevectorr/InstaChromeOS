#!/bin/bash

# Quick build script for developers
# This script does a fast build without all the checks

echo "🚀 Quick building InstaChromeOS..."

# Build kernel
cd kernel
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✅ Kernel built"
else
    echo "❌ Kernel build failed"
    exit 1
fi

cd ..

# Create ISO structure
echo "📁 Creating ISO structure..."
rm -rf iso 2>/dev/null
mkdir -p iso/boot/grub
mkdir -p iso/InstaChromeOS/{system/int_handler,personal,space,tmp}

# Copy files
cp kernel/kernel.elf iso/boot/

# Create GRUB config
cat > iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "InstaChromeOS" {
    multiboot2 /boot/kernel.elf
    boot
}
EOF

# Create system files
touch iso/InstaChromeOS/system/int_handler/int.ih
touch iso/InstaChromeOS/system/OSsys.os
touch iso/InstaChromeOS/system/Linker.hmk
touch iso/InstaChromeOS/system/instachrome.jl

# Create READMEs
echo "Personal files" > iso/InstaChromeOS/personal/README.txt
echo "Space files" > iso/InstaChromeOS/space/README.txt
echo "Temp files" > iso/InstaChromeOS/tmp/README.txt

# Create ISO
echo "💿 Creating ISO..."
grub-mkrescue -o InstaChromeOS.iso iso 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ ISO created: InstaChromeOS.iso"
    echo "📊 Size: $(du -h InstaChromeOS.iso | cut -f1)"
else
    echo "❌ ISO creation failed"
    exit 1
fi

echo ""
echo "🎉 Build complete! Run with: qemu-system-i386 -cdrom InstaChromeOS.iso"