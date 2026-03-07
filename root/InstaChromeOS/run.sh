#!/bin/bash

# Quick run script
# This script builds and runs the OS in one command

echo "🏃 Running InstaChromeOS..."

# Check if ISO exists
if [ ! -f "InstaChromeOS.iso" ]; then
    echo "📦 ISO not found, building first..."
    ./quick_build.sh
fi

# Run in QEMU
echo "🚀 Starting QEMU..."
echo "📝 Press Ctrl+Alt+G to release mouse, then Ctrl+A then X to exit"
echo ""
qemu-system-i386 -cdrom InstaChromeOS.iso -m 128M -vga std -enable-kvm