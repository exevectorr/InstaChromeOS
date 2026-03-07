#!/bin/bash

# Clean script
echo "🧹 Cleaning InstaChromeOS build files..."

# Remove build directories
rm -rf iso 2>/dev/null
rm -f InstaChromeOS.iso 2>/dev/null

# Clean kernel
cd kernel
make clean > /dev/null 2>&1
cd ..

echo "✅ Clean complete!"