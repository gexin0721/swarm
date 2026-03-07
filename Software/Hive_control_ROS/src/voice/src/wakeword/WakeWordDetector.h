#pragma once
#include <string>
#include <functional>

class WakeWordDetector {
public:
    using WakeWordCallback = std::function<void(const std::string&)>;
    
    WakeWordDetector(const std::string& modelPath);
    ~WakeWordDetector();
    
    void start();
    void stop();
    void setCallback(WakeWordCallback callback);
    
private:
    std::string modelPath_;
    WakeWordCallback callback_;
    void* detectorHandle_;
    bool isRunning_;
};
