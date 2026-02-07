#pragma once
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <memory>

class ASR;
class AudioCapture;
class WakeWordDetector;
class TTS;

/**
 * 语音服务主控制器
 * 协调ASR、音频采集、唤醒词检测和TTS各个模块
 */
class VoiceService {
public:
    VoiceService(ros::NodeHandle& nh);
    ~VoiceService();
    
    // 启动服务
    bool start();
    
    // 停止服务
    void stop();
    
private:
    // ROS回调函数
    void wakeWordCallback(const std_msgs::String::ConstPtr& msg);
    void ttsTextCallback(const std_msgs::String::ConstPtr& msg);
    
    // 音频处理回调
    void onAudioData(const std::vector<int16_t>& audio);
    
    // 发布识别结果
    void publishRecognitionResult(const std::string& text);
    
    // 发布TTS状态
    void publishTTSStatus(const std::string& status);
    
    // 加载配置
    bool loadConfig();
    
    ros::NodeHandle nh_;
    ros::Publisher recognitionResultPub_;
    ros::Publisher ttsStatusPub_;
    ros::Subscriber wakewordSub_;
    ros::Subscriber ttsTextSub_;
    
    std::unique_ptr<ASR> asr_;
    std::unique_ptr<AudioCapture> audioCapture_;
    std::unique_ptr<WakeWordDetector> wakeWordDetector_;
    std::unique_ptr<TTS> tts_;
    
    bool isRunning_;
};
