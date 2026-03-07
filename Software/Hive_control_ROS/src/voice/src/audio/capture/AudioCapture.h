#pragma once
#include <vector>
#include <cstdint>
#include <functional>

class AudioCapture {
public:
    using AudioCallback = std::function<void(const std::vector<int16_t>&)>;
    
    AudioCapture(int sampleRate = 16000, int channels = 1);
    ~AudioCapture();
    
    void start();
    void stop();
    void setCallback(AudioCallback callback);
    
private:
    int sampleRate_;
    int channels_;
    AudioCallback callback_;
    void* audioHandle_;
    bool isRunning_;
};
