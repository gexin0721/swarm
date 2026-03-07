#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * 语音识别模块（基于FunASR）
 * 提供高精度的中文语音识别功能
 */
class ASR {
public:
    struct Config {
        std::string modelPath;
        int sampleRate;
        std::string language;
    };
    
    ASR(const Config& config);
    ~ASR();
    
    // 初始化识别引擎
    bool initialize();
    
    // 关闭识别引擎
    void shutdown();
    
    // 识别音频数据
    // @param audio: 音频采样数据（16bit PCM）
    // @return: 识别出的文本
    std::string recognize(const std::vector<int16_t>& audio);
    
    // 流式识别（可选实现）
    void startStream();
    void feedAudio(const std::vector<int16_t>& audio);
    std::string getStreamResult();
    void stopStream();
    
    // 检查是否已初始化
    bool isInitialized() const { return initialized_; }
    
private:
    Config config_;
    void* modelHandle_;      // FunASR模型句柄
    bool initialized_;
    bool streaming_;
    
    // 内部辅助方法
    bool loadModel();
    void unloadModel();
};
