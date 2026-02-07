# DroneSwarm 智能无人机集群控制系统

基于ESP32-S3的无人机控制系统，集成GPS定位、姿态控制、语音指挥和AI协作功能，构建智能化、可扩展的无人机集群平台

## 项目架构

本项目采用**智能化四层架构设计**
```
┌─────────────────┐    WiFi-UDP    ┌──────────────────┐              ┌─────────────────────┐
│   无人机端      │ ◄─────────────► │    上位机ROS     │ ◄──────────► │   AI编辑器内核      │
│   (ESP32-S3)    │                │   (Ubuntu/ROS1)  │              │  (双AI协同架构)      │
└─────────────────┘                └──────────────────┘              └─────────────────────┘
        │                                    │                                │
        ▼                                    ▼                                ▼
  姿态控制+GPS定位              路径规划+数据处理+监控         对话AI+知识AI+MCP工具+文件监控
                                                                              │
                                                          ┌───────────────────┴───────────────────┐
                                                          ▼                                       ▼
                                                   语音交互系统                              工具调用系统
                                              (FunASR识别+TTS合成)                      (30+工具函数)
```

## 项目结构

```
DroneSwarm/
├── docs/                       # 📋 项目文档
│   └── 技术路线规划.md          # 详细的系统设计文档
├── Hardware/                   # 🔧 硬件设计文件
│   ├── CDA/                    # 3D机身模型
│   └── PCB/                    # 电路板设计
├── Software/                   # 💻 软件代码
│   ├── UAV_ESP/                # ✅ ESP32固件 (开发中)
│   │   ├── Hardware/           # 硬件驱动模块
│   │   ├── System/             # 系统功能模块
│   │   └── main/               # 主程序
│   └── Hive_control_ROS/       # 🚀 ROS智能上位机 (功能丰富)
│       └── src/                # ROS功能包
│           ├── swarm_*/        # 无人机核心功能包
│           ├── AI/             # 🤖 AI编辑器内核 (全新架构)
│           │   └── scripts/    # AI核心模块
│           │       ├── ai_assistant.py      # 主程序入口
│           │       ├── module/              # 核心模块目录
│           │       │   ├── Agent/           # AI代理模块（三阶段对话流程）
│           │       │   │   ├── ConversationHandler.py  # 对话处理器
│           │       │   │   ├── _Search.py   # 知识检索处理
│           │       │   │   └── _Dialogue.py # 对话生成处理
│           │       │   ├── AICore/          # AI核心模块
│           │       │   │   ├── AIManager.py # AI工厂（双AI协同管理）
│           │       │   │   ├── Model/       # 多模型支持
│           │       │   │   │   ├── deepseek.py  # DeepSeek模型 ✓
│           │       │   │   │   ├── qwen.py      # 通义千问模型 ✓
│           │       │   │   │   ├── Kiimi.py     # Kimi模型 ✓
│           │       │   │   │   ├── doubao.py    # 豆包模型 ✓
│           │       │   │   │   └── ...          # Claude/Gemini等(待实现)
│           │       │   │   ├── Tool/        # AI工具类
│           │       │   │   │   ├── OPEN_AI.py   # OpenAI标准接口封装
│           │       │   │   │   └── HistoryManager.py  # 历史管理器
│           │       │   │   └── role/        # 角色配置
│           │       │   │       ├── config.json      # 模型配置
│           │       │   │       ├── secret_key.json  # API密钥
│           │       │   │       ├── role_A/  # 对话AI配置
│           │       │   │       └── role_B/  # 知识AI配置
│           │       │   └── MCP/             # MCP工具调用系统
│           │       │       ├── server/      # MCP服务端（30+工具）
│           │       │       │   ├── MCPServer.py
│           │       │       │   └── Tools/   # 工具集
│           │       │       └── client/      # MCP客户端
│           │       ├── tools/               # 通用工具
│           │       │   ├── log.py           # 日志系统
│           │       │   └── AllEventsHandler.py  # 文件监控器
│           │       ├── window/              # Web配置管理界面
│           │       ├── Data/                # 数据存储
│           │       └── test/                # 测试文件
│           ├── voice/          # 🎤 语音交互系统 (新架构)
│           │   ├── src/        # C++源代码
│           │   │   ├── asr/    # FunASR语音识别
│           │   │   ├── tts/    # 文本转语音
│           │   │   ├── audio/  # 音频采集&处理
│           │   │   └── wakeword/ # 唤醒词检测
│           │   └── models/     # 语音模型文件
│           └── 环境/            # ⚡ 一键配置脚本
│               ├── robot.sh    # 智能环境安装脚本
│               └── ch9102_udev.sh  # 串口设备权限
└── test/                       # 🧪 测试文件
```

## 开发状态

> 🚨 **注意**: 这是个人学习项目，大部分代码还在开发中，很多模块只是搭建了基础框架

### 📊 模块完成度一览

| 模块 | 完成度 | 状态说明 |
|------|--------|----------|
| ESP32硬件驱动 | 90% | 12个驱动模块已实现 |
| 飞控算法(PID/Kalman) | 80% | 框架完整，待参数调优 |
| AI助手内核 | 85% | 双AI架构+30+MCP工具 |
| ROS-UDP桥接 | 70% | 已实现基础通信 |
| 集群规划器 | 20% | 框架搭建阶段 |
| GUI监控 | 20% | 框架搭建阶段 |
| 语音模块 | 30% | 框架搭建阶段 |

**整体完成度：约55-60%**

### 🚧 当前开发中
- **ESP32固件框架** - 基于ESP-IDF，部分复用以前代码
  - ✅ 基础WiFi/UDP通信框架
  - ✅ GPS模块初始化（最新完成）
  - 🏗️ 硬件驱动模块（13个模块框架已建立）
    - 传感器接口定义（GPS/JY60/MPU6050等）
    - 执行器控制接口（PWM/LED/蜂鸣器等）
  - 🏗️ 系统算法模块
    - PID控制器（基础实现）
    - 卡尔曼滤波（框架搭建）
  - ✅ 编译构建系统

### 🚀 **新增亮点功能** - 智能交互系统

#### 🤖 AI编辑器内核（全新架构）
- **双AI协同架构**
  - ✅ 对话AI（role_A）：负责理解用户需求、规划任务、生成回答
  - ✅ 知识AI（role_B）：负责数据收集、文件搜索、简单处理
  - ✅ 解耦设计：业务层通过回调调用，不直接持有AI实例

- **三阶段对话流程**
  - ✅ 阶段1：知识AI生成TODO搜索规划
  - ✅ 阶段2：知识AI执行TODO列表（调用MCP工具/扫描文件）
  - ✅ 阶段3：对话AI基于查询结果生成最终回答
  - ✅ ConversationHandler统一编排流程

- **MCP工具调用系统**
  - ✅ 30+工具函数（文件操作、数据库操作、数据查询）
  - ✅ 异步任务队列管理
  - ✅ JSON指令格式调用
  - 🚧 权限管理系统（待实现）

- **文件监控与工作区索引**
  - ✅ 实时监控工作区文件变化（基于watchdog）
  - ✅ 事件队列管理
  - ✅ 自动更新文件上下文并注入到知识AI

- **OPEN_AI统一接口**
  - ✅ 统一的大模型API封装（支持OpenAI标准接口）
  - ✅ 智能历史管理系统（自动Token裁剪、保留system消息）
  - ✅ 支持流式和非流式输出
  - ✅ 完整的错误处理和日志记录

- **多模型支持**
  - ✅ DeepSeek - 已实现
  - ✅ Qwen（通义千问）- 已实现
  - ✅ Kimi - 已实现
  - ✅ Doubao（豆包）- 已实现
  - 🚧 Claude / Gemini / ChatGPT / Xinhuo（待实现）

#### 🎤 语音交互系统（新架构）
- **Voice 包 - 基于 FunASR**
  - ✅ 高精度中文语音识别（FunASR 引擎）
  - ✅ 唤醒词检测（低功耗持续监听）
  - ✅ TTS 文本转语音合成
  - ✅ 音频采集与预处理（噪声抑制、VAD）
  - ✅ C++ 实现，性能优秀
  - 🚧 流式识别（开发中）

### 📋 ROS智能上位机部分
- **核心无人机功能包** - 基础框架，待完善实现
  - 📦 `udp_ros_bridge` - UDP↔ROS话题转换（有基础实现）
  - 📦 `swarm_data_processor` - 数据处理（框架搭建）
  - 📦 `swarm_planner` - 路径规划（框架搭建）
  - 📦 `swarm_monitor` - 监控可视化（框架搭建）

- **🎯 智能交互功能包** - 全新架构，功能完备
  - 📦 `AI` - AI编辑器内核（✅ 全新架构实现）
    - `module/Agent/` - AI代理模块（三阶段对话流程编排）
      - `ConversationHandler.py` - 对话处理器
      - `_Search.py` - 知识检索处理
      - `_Dialogue.py` - 对话生成处理
    - `module/AICore/` - AI核心模块
      - `AIManager.py` - AI工厂（双AI协同管理）
      - `Model/` - 多模型支持（DeepSeek/Qwen/Kimi/Doubao等）
      - `Tool/` - AI工具类（OPEN_AI/HistoryManager）
      - `role/` - 角色配置（role_A对话AI / role_B知识AI）
    - `module/MCP/` - MCP工具调用系统（30+工具函数）
      - `server/` - MCP服务端（文件/数据库/数据查询工具）
      - `client/` - MCP客户端（异步任务队列）
    - `tools/` - 通用工具（日志系统/文件监控器）
    - `window/` - Web配置管理界面
  - 📦 `voice` - 语音交互系统（✅ 新架构实现）
    - `asr/` - FunASR 高精度中文识别
    - `tts/` - 文本转语音合成
    - `audio/` - 音频采集与处理
    - `wakeword/` - 唤醒词检测

### 🎯 下一步计划
- **硬件集成阶段**
  - 完善ESP32各硬件模块的具体实现
  - 集成测试WiFi通信和传感器数据读取
  - 硬件调试和PID参数调优

- **智能化升级阶段** 🌟
  - 将语音AI系统与无人机控制逻辑深度融合
  - 实现自然语言飞行指令解析（"飞到那边"→坐标转换）
  - 开发AI辅助的集群协作决策算法
  - 集成语音状态播报和异常告警系统

- **系统联调阶段**
  - 语音指挥→AI理解→路径规划→飞行控制全链路测试
  - 多机协同语音指挥场景验证
  - 安全机制和应急语音指令实现

## 设计目标

### 🎯 ESP32固件端（规划中）
- **多传感器融合**：JY60姿态+NEO-M8N GPS+电池监控
- **高频控制循环**：目标100Hz姿态控制，50Hz数据上传
- **实时参数调试**：UART0接口支持PID参数实时调整
- **资源优化设计**：硬件资源精确分配，避免冲突

### 🖥️ ROS智能上位机端（功能丰富）
- **模块化架构**：七个功能包各司其职，便于扩展
- **坐标系转换**：GPS经纬度→UTM坐标，便于路径计算
- **实时监控**：电池电压、GPS信号强度、飞行状态
- **可视化界面**：RViz显示飞行轨迹和系统状态

### 🤖 **AI编辑器内核端**（已实现核心架构）
- **双AI协同系统**：对话AI（理解需求、生成回答）+ 知识AI（数据收集、文件搜索）
- **三阶段对话流程**：TODO规划 → 执行查询 → 生成回答
- **MCP工具调用**：30+工具函数，支持文件/数据库/数据查询操作
- **文件监控系统**：实时监控工作区变化，自动更新上下文
- **多模型支持**：DeepSeek、Qwen、Kimi、Doubao等多种大模型
- **智能历史管理**：自动Token裁剪，保留关键上下文

### 🎤 **语音交互系统端**（已实现核心功能）
- **语音识别系统**：FunASR引擎，高精度中文识别
- **唤醒词检测**：低功耗持续监听，支持自定义唤醒词
- **语音合成系统**：TTS引擎，多角色语音播报
- **音频处理**：噪声抑制、VAD语音活动检测

## 快速开始

### 环境要求
- **无人机端**：ESP-IDF v5.0+
- **上位机端**：Ubuntu 20.04 + ROS Noetic
- **硬件**：ESP32-S3 + JY60 + NEO-M8N + OLED显示屏
- **AI对话系统**：
  - Python 3.8+
  - transformers 库（Qwen tokenizer）
  - 各大模型 API 密钥
- **语音交互系统**：
  - FunASR 引擎（离线识别）
  - 麦克风设备（支持 ALSA）
  - jsoncpp / ALSA 开发库
  - ⚡ **一键配置脚本**：`robot.sh`自动安装所有依赖

### 编译ESP32固件
```bash
cd Software/UAV_ESP
idf.py build
idf.py flash monitor
```

### 编译ROS功能包
```bash
cd Software/Hive_control_ROS
catkin_make
source devel/setup.bash
```

### 🔧 一键环境配置
**重要：首次使用前必须运行环境配置脚本**
```bash
cd Software/Hive_control_ROS/环境
sudo chmod +x robot.sh
sudo ./robot.sh
```
**此脚本将自动完成：**
- ✅ 安装系统依赖包（音频库、Python包等）
- ✅ 下载 FunASR 模型文件
- ✅ 配置音频设备权限
- ✅ 安装 jsoncpp / ALSA 开发库
- ✅ 创建必要的目录结构

### 🤖 使用AI编辑器系统

#### 配置API密钥

**方式一：使用Web配置界面（推荐）**
```bash
cd Software/Hive_control_ROS/src/AI/scripts/window
python app.py
# 或直接双击：启动配置管理.bat
```
在浏览器中打开 `http://localhost:5000`，配置您的API密钥。

**方式二：手动编辑配置文件**
编辑 `Software/Hive_control_ROS/src/AI/scripts/module/AICore/role/secret_key.json`：
```json
{
  "deepseek": {
    "api_key": "your_deepseek_api_key_here",
    "base_url": "https://api.deepseek.com",
    "model": "deepseek-chat"
  },
  "qwen": {
    "api_key": "your_qwen_api_key_here",
    "base_url": "https://dashscope.aliyuncs.com/compatible-mode/v1",
    "model": "qwen-turbo"
  }
}
```

#### 运行AI编辑器
```bash
cd Software/Hive_control_ROS/src/AI/scripts
python ai_assistant.py
```

#### 核心特性说明
- **双AI协同**：对话AI负责理解和回答，知识AI负责数据收集
- **三阶段流程**：自动规划TODO → 执行查询 → 生成回答
- **MCP工具调用**：支持文件操作、数据库查询、数据分析等30+工具
- **文件监控**：实时监控工作区文件变化，自动更新上下文

详细文档：
- `Software/Hive_control_ROS/src/AI/scripts/README.md` - 业务逻辑说明
- `Software/Hive_control_ROS/src/AI/scripts/PROJECT_STRUCTURE.md` - 项目结构说明

### 🎤 使用语音交互系统

#### 启动语音服务
```bash
# 启动语音系统
roslaunch voice voice.launch
```

#### ROS 话题
```bash
# 发布话题
/voice/recognition_result  # 识别结果
/voice/tts_status          # TTS 状态

# 订阅话题
/voice/wakeword            # 唤醒词触发
/voice/tts_text            # TTS 文本输入
```

详细文档：`Software/Hive_control_ROS/src/voice/README.md`

## 硬件配置

| 模块 | 型号 | 接口 | 功能 |
|------|------|------|------|
| 主控 | ESP32-S3 | - | 双核处理器，240MHz |
| 姿态传感器 | JY60 | UART2 | 三轴姿态数据 |
| GPS模块 | NEO-M8N | UART1 | 室外定位 |
| 显示屏 | 0.96" OLED | I2C1 | 状态显示 |
| 电机驱动 | 8520空心杯×4 | PWM | 四轴推进 |
| **语音设备** | **麦克风** | **ALSA** | **语音指挥输入** |
| **AI处理** | **通义千问/星火API** | **HTTP/WebSocket** | **智能对话决策** |

## 通信协议

### UDP数据格式（JSON）
```json
{
  "drone_id": "test_01",
  "roll": 1.1, "pitch": 0.8, "yaw": 359.2,
  "lat": 30.123456, "lon": 120.654321,
  "sat": 13, "batt": 3.85,
  "pid_p": 2.5, "pid_i": 0.3, "pid_d": 0.8
}
```

### 🎤 语音+AI 通信流程
```
用户语音 → Voice包(FunASR识别) → /voice/recognition_result
                                              ↓
                                        AI包(智能理解)
                                              ↓
                                        /cmd/voice (飞行指令)
                                              ↓
                                        swarm_planner (路径规划)
                                              ↓
                                        /voice/tts_text (TTS反馈)
```

### ROS话题通信
```bash
# Voice 包话题
/voice/recognition_result  # 识别结果 (std_msgs/String)
/voice/tts_text           # TTS文本输入 (std_msgs/String)
/voice/tts_status         # TTS状态 (std_msgs/String)
/voice/wakeword           # 唤醒词触发 (std_msgs/String)

# AI 编辑器内核 (独立Python应用)
# 作为独立的AI编辑器工具使用，不直接发布ROS话题
# 核心功能：
#   - 双AI协同对话（对话AI + 知识AI）
#   - 三阶段对话流程（TODO规划 → 执行 → 回答）
#   - MCP工具调用（文件/数据库/数据查询）
#   - 文件监控与工作区索引

# 无人机控制话题
/cmd/voice                # 语音飞行指令 (geometry_msgs/Twist)
/drone_status             # 无人机状态反馈 (自定义消息)
```

### 网络配置
- **ESP32 IP**：192.168.1.101（固定）
- **上位机端口**：8888（UDP）
- **数据频率**：50Hz上传，20Hz下载
- **AI API**：HTTP 连接（支持 DeepSeek、Qwen、Kimi、Doubao 等）
- **语音识别**：FunASR 离线处理，无需网络连接
- **MCP工具**：本地调用，支持文件/数据库/数据查询操作

## 开发笔记

这是个人学习和研究无人机控制系统的项目，目标是：
- 深入理解ESP32嵌入式开发
- 学习ROS机器人操作系统
- 掌握多传感器融合和控制算法
- 探索无人机集群协同控制

### 🚀 **项目亮点升级**
全新架构的AI编辑器内核与智能交互系统：

#### 🤖 AI编辑器内核（核心创新）
- **双AI协同架构**：对话AI + 知识AI 分工协作
  - 对话AI（role_A）：理解用户需求、规划任务、生成回答
  - 知识AI（role_B）：数据收集、文件搜索、简单处理
  - 解耦设计：业务层通过回调调用，不直接持有AI实例

- **三阶段对话流程**：智能化任务处理
  - 阶段1：知识AI生成TODO搜索规划
  - 阶段2：知识AI执行TODO列表（调用MCP工具/扫描文件）
  - 阶段3：对话AI基于查询结果生成最终回答

- **MCP工具调用系统**：30+工具函数
  - 文件操作（读取、编辑、删除、搜索等）
  - 数据库操作（SQLite增删改查）
  - 数据查询（模糊搜索、统计、条件筛选等）
  - 异步任务队列管理

- **文件监控与工作区索引**：实时上下文更新
  - 基于watchdog的文件系统监控
  - 自动捕捉文件创建、修改、删除事件
  - 动态更新文件上下文并注入到知识AI

- **智能历史管理**：高效上下文管理
  - 自动Token计算与裁剪
  - 保留关键system消息
  - 支持多角色历史分离（role_A/role_B）

- **多模型支持**：灵活切换大模型
  - 已支持：DeepSeek、Qwen、Kimi、Doubao
  - 待支持：Claude、Gemini、ChatGPT、Xinhuo

#### 🎤 语音交互系统
- **高性能语音系统**：基于FunASR的C++实现
  - 高精度中文识别（离线运行）
  - 唤醒词检测（低功耗监听）
  - TTS语音合成反馈
  - 音频预处理（噪声抑制、VAD）

#### 🚁 未来集成方向
- **自然语言飞行控制**："飞到那边" → AI理解 → GPS坐标 → 自动飞行
- **智能状态播报**：实时语音播报飞行状态、电池电量、GPS信号
- **多机协同指挥**："所有无人机返航" → 集群统一控制

## 项目状态

- **当前版本**: v0.4.1-beta（GPS初始化完成版）
- **开发阶段**: AI编辑器内核架构完成，ESP32 GPS模块初始化完成
- **更新日期**: 2026-02-02
- **核心特色**:
  - ✅ AI编辑器内核（双AI协同架构）
    - ✅ 三阶段对话流程（TODO规划 → 执行 → 回答）
    - ✅ MCP工具调用系统（30+工具函数）
    - ✅ 文件监控与工作区索引
    - ✅ 智能历史管理（自动Token裁剪）
    - ✅ 多模型支持（DeepSeek/Qwen/Kimi/Doubao）
  - ✅ 高性能语音系统（Voice包+FunASR）
  - 🚧 AI编辑器与无人机控制集成（规划中）
  - 🚧 语音+AI深度融合（规划中）

如果你对无人机控制、ESP32开发或ROS系统感兴趣，欢迎参考本项目的代码和文档。