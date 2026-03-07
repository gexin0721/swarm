# Voice Package

无人机集群语音系统 - 简化版架构

使用FunASR提供高精度中文语音识别，集成唤醒词检测和TTS功能。

---

## 📚 目录结构

```
voice/
│
├── 📄 CMakeLists.txt              # CMake构建配置
├── 📄 package.xml                 # ROS包定义
├── 📄 README.md                   # 本文档
│
├── 📁 src/                        # 源代码目录
│   │
│   ├── 📄 main.cpp                # 主程序入口
│   │
│   ├── 📁 asr/                    # 【语音识别模块】- FunASR
│   │   ├── ASR.h                  # ASR接口
│   │   │                          # - 高精度中文识别
│   │   │                          # - 支持流式/非流式识别
│   │   └── ASR.cpp                # ASR实现
│   │                              # - 基于阿里FunASR引擎
│   │
│   ├── 📁 audio/                  # 【音频处理模块】
│   │   ├── 📁 capture/            # 音频采集
│   │   │   ├── AudioCapture.h
│   │   │   └── AudioCapture.cpp
│   │   └── 📁 process/            # 音频预处理
│   │       ├── AudioProcess.h     # - 噪声抑制
│   │       └── AudioProcess.cpp   # - 音频归一化、VAD
│   │
│   ├── 📁 wakeword/               # 【唤醒词检测模块】
│   │   ├── WakeWordDetector.h     # 唤醒词检测器
│   │   └── WakeWordDetector.cpp   # - 低功耗持续监听
│   │
│   ├── 📁 tts/                    # 【文本转语音模块】
│   │   ├── TTS.h                  # TTS引擎接口
│   │   └── TTS.cpp                # - 中英文语音合成
│   │
│   ├── 📁 serve/                  # 【ROS服务层】
│   │   ├── VoiceService.h         # 语音服务主控制器
│   │   └── VoiceService.cpp       # - 协调各模块工作
│   │
│   └── 📁 config/                 # 【配置文件】
│       ├── voice_config.json      # 语音系统配置
│       └── secret_key.json        # API密钥
│
├── 📁 scripts/                    # Python工具脚本
│   └── test_audio.py              # 测试脚本
│
├── 📁 launch/                     # ROS启动文件
│   └── voice.launch               # 启动配置
│
└── 📁 models/                     # 模型文件
    ├── 📁 funasr/                 # FunASR模型
    └── 📁 wakeword/               # 唤醒词模型
```

---

## 🏗️ 系统架构（简化版）

```
         main.cpp
            ↓
      VoiceService (统一控制)
            ↓
    ┌───────┼───────┬─────────┐
    ↓       ↓       ↓         ↓
  ASR   Audio  WakeWord    TTS
(FunASR) Capture Detector
```

### 设计理念

- **简单直接**：去除工厂模式和抽象层
- **单一引擎**：直接使用FunASR（高精度中文识别）
- **易于维护**：代码结构清晰，模块职责明确
- **高效执行**：无运行时开销，编译后直接运行

---

## 🚀 使用指南

### 1. 安装FunASR

```bash
# 参考FunASR官方文档
# https://github.com/alibaba-damo-academy/FunASR

# Python版本
pip install funasr

# C++版本需要编译安装
```

### 2. 编译

```bash
cd ~/Desktop/DroneSwarm/Software/Hive_control_ROS
catkin_make
```

### 3. 配置

编辑 `src/config/voice_config.json`：

```json
{
    "asr": {
        "model_path": "./models/funasr",
        "sample_rate": 16000,
        "language": "zh"
    }
}
```

### 4. 启动

```bash
roslaunch voice voice.launch
```

---

## 📡 ROS接口

### 发布话题

| 话题 | 类型 | 说明 |
|------|------|------|
| `/voice/recognition_result` | String | 识别结果 |
| `/voice/tts_status` | String | TTS状态 |

### 订阅话题

| 话题 | 类型 | 说明 |
|------|------|------|
| `/voice/wakeword` | String | 唤醒词触发 |
| `/voice/tts_text` | String | TTS文本输入 |

---

## ⚙️ 依赖项

- ROS (Melodic/Noetic)
- ALSA: `sudo apt-get install libasound2-dev`
- jsoncpp: `sudo apt-get install libjsoncpp-dev`
- FunASR: 见官方文档

---

## 🔧 开发说明

### 为什么选择FunASR？

- ✅ 中文识别准确率最高
- ✅ 支持流式/非流式识别
- ✅ 开源免费，可离线运行
- ✅ 性能优秀，资源占用低

### 为什么不用工厂模式？

语音识别不同于AI对话：
- 识别引擎选最好的就行，基本不换
- 无需运行时切换
- 保持代码简洁

如需更换引擎，直接修改ASR.cpp实现即可。

---

## 📝 TODO

- [ ] 集成FunASR C++ API
- [ ] 实现流式识别
- [ ] 完善音频预处理
- [ ] 优化唤醒词检测
- [ ] 性能测试和优化

---

## 📄 许可

MIT License
