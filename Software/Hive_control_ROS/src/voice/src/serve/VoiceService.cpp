#include "VoiceService.h"
#include "../asr/ASR.h"
#include "../audio/capture/AudioCapture.h"
#include "../wakeword/WakeWordDetector.h"
#include "../tts/TTS.h"
#include <iostream>
#include <fstream>
#include <json/json.h>

VoiceService::VoiceService(ros::NodeHandle& nh) 
    : nh_(nh), isRunning_(false) {
    
    // 初始化ROS发布者和订阅者
    recognitionResultPub_ = nh_.advertise<std_msgs::String>("voice/recognition_result", 10);
    ttsStatusPub_ = nh_.advertise<std_msgs::String>("voice/tts_status", 10);
    
    wakewordSub_ = nh_.subscribe("voice/wakeword", 10, 
                                  &VoiceService::wakeWordCallback, this);
    ttsTextSub_ = nh_.subscribe("voice/tts_text", 10,
                                 &VoiceService::ttsTextCallback, this);
    
    std::cout << "VoiceService initialized" << std::endl;
}

VoiceService::~VoiceService() {
    stop();
}

bool VoiceService::start() {
    if (isRunning_) {
        std::cout << "VoiceService already running" << std::endl;
        return true;
    }
    
    std::cout << "Starting voice service..." << std::endl;
    
    // 加载配置
    if (!loadConfig()) {
        std::cerr << "Failed to load config" << std::endl;
        return false;
    }
    
    // 初始化ASR
    ASR::Config asrConfig;
    asrConfig.modelPath = "./models/funasr";  // 从配置文件读取
    asrConfig.sampleRate = 16000;
    asrConfig.language = "zh";
    
    asr_ = std::make_unique<ASR>(asrConfig);
    if (!asr_->initialize()) {
        std::cerr << "Failed to initialize ASR" << std::endl;
        return false;
    }
    
    // TODO: 初始化其他模块
    // audioCapture_ = std::make_unique<AudioCapture>(16000, 1);
    // wakeWordDetector_ = std::make_unique<WakeWordDetector>("./models/wakeword");
    // tts_ = std::make_unique<TTS>("./config/tts_config.json");
    
    isRunning_ = true;
    std::cout << "Voice service started successfully" << std::endl;
    return true;
}

void VoiceService::stop() {
    if (!isRunning_) {
        return;
    }
    
    std::cout << "Stopping voice service..." << std::endl;
    
    // 停止所有模块
    if (asr_) {
        asr_->shutdown();
        asr_.reset();
    }
    
    if (audioCapture_) {
        audioCapture_->stop();
        audioCapture_.reset();
    }
    
    if (wakeWordDetector_) {
        wakeWordDetector_->stop();
        wakeWordDetector_.reset();
    }
    
    if (tts_) {
        tts_->shutdown();
        tts_.reset();
    }
    
    isRunning_ = false;
    std::cout << "Voice service stopped" << std::endl;
}

void VoiceService::wakeWordCallback(const std_msgs::String::ConstPtr& msg) {
    std::cout << "Wake word detected: " << msg->data << std::endl;
    
    // TODO: 开始录音和识别
    // 1. 启动音频采集
    // 2. 收集音频数据
    // 3. 调用ASR识别
    // 4. 发布识别结果
}

void VoiceService::ttsTextCallback(const std_msgs::String::ConstPtr& msg) {
    std::cout << "TTS request: " << msg->data << std::endl;
    
    if (tts_) {
        tts_->speak(msg->data);
        publishTTSStatus("speaking");
    }
}

void VoiceService::onAudioData(const std::vector<int16_t>& audio) {
    if (asr_ && asr_->isInitialized()) {
        std::string result = asr_->recognize(audio);
        if (!result.empty()) {
            publishRecognitionResult(result);
        }
    }
}

void VoiceService::publishRecognitionResult(const std::string& text) {
    std_msgs::String msg;
    msg.data = text;
    recognitionResultPub_.publish(msg);
    std::cout << "Published recognition result: " << text << std::endl;
}

void VoiceService::publishTTSStatus(const std::string& status) {
    std_msgs::String msg;
    msg.data = status;
    ttsStatusPub_.publish(msg);
}

bool VoiceService::loadConfig() {
    // TODO: 从配置文件加载参数
    std::cout << "Loading config from: src/config/voice_config.json" << std::endl;
    return true;
}
