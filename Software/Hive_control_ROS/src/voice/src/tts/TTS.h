#pragma once
#include <string>

class TTS {
public:
    TTS(const std::string& configPath);
    ~TTS();
    
    void initialize();
    void shutdown();
    void speak(const std::string& text);
    
private:
    std::string configPath_;
    void* ttsHandle_;
};
