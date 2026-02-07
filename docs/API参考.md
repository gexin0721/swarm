# DroneSwarm API参考文档

本文档提供系统各模块的接口说明。

---

## 一、MQTT通信协议

### 1.1 Topic设计

| Topic | 方向 | QoS | 说明 |
|-------|------|-----|------|
| `swarm/{id}/data` | 无人机→上位机 | 0 | 实时数据 |
| `swarm/{id}/cmd` | 上位机→无人机 | 1 | 单机控制 |
| `swarm/all/cmd` | 上位机→所有 | 1 | 集群广播 |
| `swarm/{id}/status` | 无人机→上位机 | 1 | 状态变更 |

### 1.2 数据上报格式

**Topic**: `swarm/{id}/data`

```json
{
  "ts": 1706860800000,
  "roll": 1.1,
  "pitch": 0.8,
  "yaw": 359.2,
  "lat": 30.123456,
  "lon": 120.654321,
  "sat": 13,
  "batt": 3.85
}
```

### 1.3 字段说明

| 字段 | 类型 | 单位 | 说明 |
|------|------|------|------|
| ts | int64 | ms | 时间戳 |
| roll | float | 度 | 横滚角 |
| pitch | float | 度 | 俯仰角 |
| yaw | float | 度 | 偏航角 |
| lat | float | 度 | GPS纬度 |
| lon | float | 度 | GPS经度 |
| sat | int | - | GPS卫星数量 |
| batt | float | V | 电池电压 |

### 1.4 控制指令格式

**Topic**: `swarm/{id}/cmd`

```json
{
  "type": "move",
  "target": {"lat": 30.124, "lon": 120.655, "alt": 10}
}
```

**指令类型**：
| type | 说明 |
|------|------|
| move | 移动到目标位置 |
| hover | 悬停 |
| land | 降落 |
| return | 返航 |

### 1.5 网络配置

| 参数 | 值 |
|------|-----|
| Broker地址 | localhost |
| 端口 | 1883 |
| 协议 | MQTT v3.1.1 |
| 上传频率 | 50Hz |

---

## 二、ROS话题/服务

### 2.1 语音系统话题

| 话题名称 | 消息类型 | 方向 | 说明 |
|----------|----------|------|------|
| /voice/recognition_result | std_msgs/String | 发布 | 语音识别结果 |
| /voice/tts_text | std_msgs/String | 订阅 | TTS文本输入 |
| /voice/tts_status | std_msgs/String | 发布 | TTS播放状态 |
| /voice/wakeword | std_msgs/String | 发布 | 唤醒词触发 |

### 2.2 无人机控制话题

| 话题名称 | 消息类型 | 方向 | 说明 |
|----------|----------|------|------|
| /cmd/voice | geometry_msgs/Twist | 发布 | 语音飞行指令 |
| /drone_status | 自定义 | 发布 | 无人机状态 |
| /swarm/path | nav_msgs/Path | 发布 | 规划路径 |

### 2.3 使用示例

```bash
# 发布TTS文本
rostopic pub /voice/tts_text std_msgs/String "data: '起飞成功'"

# 监听识别结果
rostopic echo /voice/recognition_result
```

---

## 三、AI助手接口

### 3.1 单AI架构

**入口文件**：`src/AI/scripts/ai_assistant.py`

**架构**：
```
用户输入 → 单AI理解 → MCP工具调用 → 执行结果
```

### 3.2 支持的模型

| 模型 | 配置键 | 状态 |
|------|--------|------|
| DeepSeek | deepseek | ✅ 推荐 |
| 通义千问 | qwen | ✅ 已实现 |
| Kimi | kimi | ✅ 已实现 |

### 3.3 MCP工具函数列表（精简版）

**无人机控制类**

| 工具名 | 功能 |
|--------|------|
| drone_move | 控制无人机移动 |
| drone_hover | 悬停 |
| drone_land | 降落 |
| drone_return | 返航 |
| drone_status | 查询状态 |
| swarm_broadcast | 集群广播指令 |

---

## 四、数据结构定义

### 4.1 无人机状态结构

```c
typedef struct {
    char drone_id[32];    // 无人机ID
    float roll;           // 横滚角
    float pitch;          // 俯仰角
    float yaw;            // 偏航角
    double lat;           // GPS纬度
    double lon;           // GPS经度
    int sat;              // 卫星数量
    float batt;           // 电池电压
} DroneStatus;
```

### 4.2 配置文件格式

**secret_key.json**

```json
{
  "模型名": {
    "api_key": "API密钥",
    "base_url": "API地址",
    "model": "模型ID"
  }
}
```

---

## 附录：错误码

| 错误码 | 说明 |
|--------|------|
| 0 | 成功 |
| -1 | 通信失败 |
| -2 | 参数错误 |
| -3 | 超时 |

