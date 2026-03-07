# ROS Noetic + ODB + EMQX 开发容器

这是一个完整的 ROS 开发环境容器，集成了项目所需的所有依赖。

## 📦 包含的组件

### ROS 环境
- **ROS Noetic Desktop Full** - 完整的 ROS 桌面版
- **无人机集群相关包** - MAVROS、导航、SLAM 等
- **Gazebo 仿真器** - 机器人仿真环境

### 数据库和消息队列
- **ODB ORM** - C++ 对象关系映射库
  - odb 编译器
  - libodb-dev
  - libodb-sqlite-dev
  - libodb-boost-dev
- **EMQX 5.8** - 高性能 MQTT Broker
  - Dashboard: http://localhost:18083
  - 默认账号: admin / public
  - MQTT 端口: 1883
  - WebSocket: 8083

### 语音识别
- **Vosk** - 离线语音识别引擎
- **音频处理库** - ALSA、PortAudio、SoX

### 其他工具
- **cJSON** - JSON 解析库
- **Python 3** - 包含常用科学计算库
- **开发工具** - Git、CMake、Vim 等

## 🚀 快速开始

### 方法一：使用脚本（推荐）

**1. 构建镜像（首次使用）：**
```bash
# 双击运行
.devcontainer/build-ros-image.bat
```
⏱️ 预计时间：10-30 分钟（取决于网络速度）
💾 镜像大小：约 4-5 GB

**2. 启动容器：**
```bash
# 双击运行
.devcontainer/run-ros-container.bat
```

### 方法二：在 VS Code 中使用

1. 安装 "Dev Containers" 扩展
2. 将 `devcontainer.ros.json` 重命名为 `devcontainer.json`
3. 按 F1，选择 `Dev Containers: Reopen in Container`
4. 等待容器构建和启动

### 方法三：手动命令

**构建镜像：**
```bash
cd .devcontainer
docker build -f Dockerfile.ros -t ros-dev ..
```

**运行容器：**
```bash
# Windows
docker run -it --rm -v "%cd%:/workspace" --network=host --privileged ros-dev

# Linux/Mac
docker run -it --rm -v "$(pwd):/workspace" --network=host --privileged ros-dev
```

## 📝 容器内使用指南

### ROS 开发

**编译 ROS 工作空间：**
```bash
cd /workspace/Software/Hive_control_ROS
catkin_make
source devel/setup.bash
```

**运行 ROS 节点：**
```bash
# 启动 ROS Master
roscore

# 在新终端运行节点
rosrun <package_name> <node_name>

# 或使用 launch 文件
roslaunch <package_name> <launch_file>
```

**查看话题和节点：**
```bash
rostopic list        # 列出所有话题
rostopic echo /topic # 查看话题内容
rosnode list         # 列出所有节点
```

### EMQX MQTT Broker

**启动 EMQX：**
```bash
emqx start
```

**停止 EMQX：**
```bash
emqx stop
```

**查看状态：**
```bash
emqx status
```

**访问 Dashboard：**
- URL: http://localhost:18083
- 用户名: admin
- 密码: public

**MQTT 连接信息：**
- 主机: localhost
- 端口: 1883 (TCP)
- WebSocket: 8083

### ODB 数据库开发

**生成 ODB 代码：**
```bash
odb -d sqlite --std c++11 --generate-query --generate-schema \
    -I /workspace/Software/Hive_control_ROS/src/message_manager/include \
    /path/to/your/model.hxx
```

**编译时会自动生成：**
- `model-odb.hxx` - 头文件
- `model-odb.cxx` - 实现文件
- `model-odb.ixx` - 内联文件

### 语音识别测试

**测试 Vosk 模型：**
```bash
cd /workspace/Software/Hive_control_ROS/环境
python3 test_vosk.py moxi test.wav
```

## 🔧 常见问题

### 1. 镜像构建失败

**问题：** 网络连接超时或下载失败

**解决：**
- 检查网络连接
- 使用国内镜像源（修改 Dockerfile）
- 分段构建，注释掉失败的部分

### 2. EMQX 无法启动

**问题：** 端口被占用

**解决：**
```bash
# 检查端口占用
netstat -tulpn | grep 1883

# 修改 EMQX 配置
vim /etc/emqx/emqx.conf
```

### 3. ROS 编译错误

**问题：** 找不到依赖包

**解决：**
```bash
# 更新 ROS 包索引
apt-get update
rosdep update

# 安装缺失的依赖
rosdep install --from-paths src --ignore-src -r -y
```

### 4. ODB 编译器找不到

**问题：** CMake 报错 "ODB compiler not found"

**解决：**
```bash
# 检查 ODB 是否安装
which odb

# 如果没有，重新安装
apt-get install odb libodb-dev libodb-sqlite-dev
```

## 📂 目录结构

```
/workspace/                          # 项目根目录（挂载）
├── Software/
│   ├── Hive_control_ROS/           # ROS 工作空间
│   │   ├── src/                    # ROS 包源码
│   │   ├── build/                  # 编译输出
│   │   ├── devel/                  # 开发环境
│   │   └── 环境/                   # 环境配置脚本
│   └── UAV_ESP/                    # ESP32 固件
├── Hardware/                        # 硬件设计
└── docs/                           # 文档

/opt/ros/noetic/                    # ROS 系统文件
/usr/local/lib/                     # cJSON 等库文件
/usr/bin/odb                        # ODB 编译器
/usr/lib/emqx/                      # EMQX 安装目录
```

## 🌐 端口映射

| 端口  | 服务                | 说明                    |
|-------|---------------------|-------------------------|
| 11311 | ROS Master          | ROS 核心服务            |
| 1883  | MQTT                | EMQX MQTT 协议          |
| 8883  | MQTT SSL            | EMQX MQTT 加密连接      |
| 8083  | MQTT WebSocket      | EMQX WebSocket 连接     |
| 8084  | MQTT WebSocket SSL  | EMQX WebSocket 加密     |
| 18083 | EMQX Dashboard      | EMQX Web 管理界面       |

## 🎯 下一步

1. **启动容器** - 使用 `run-ros-container.bat`
2. **编译 ROS 工作空间** - `cd /workspace/Software/Hive_control_ROS && catkin_make`
3. **启动 EMQX** - `emqx start`
4. **测试语音识别** - 运行 Vosk 测试脚本
5. **开发你的项目** - 所有修改会实时同步到 Windows

## 💡 提示

- 容器使用 `--privileged` 模式，可以访问 USB 设备
- 使用 `--network=host` 模式，网络配置与主机一致
- 所有文件修改会实时同步到 Windows
- 容器删除后，只有 `/workspace` 的内容会保留
- ROS 环境已自动配置到 `.bashrc`

## 🆚 与基础 Linux 容器的区别

| 特性           | 基础容器 | ROS 容器 |
|----------------|----------|----------|
| 镜像大小       | ~500MB   | ~4-5GB   |
| ROS 环境       | ❌       | ✅       |
| ODB ORM        | ❌       | ✅       |
| EMQX Broker    | ❌       | ✅       |
| 语音识别       | ❌       | ✅       |
| 适用场景       | 通用开发 | ROS 项目 |

## 📚 参考资料

- [ROS Noetic 文档](http://wiki.ros.org/noetic)
- [ODB ORM 文档](https://www.codesynthesis.com/products/odb/doc/)
- [EMQX 文档](https://www.emqx.io/docs/zh/v5.8/)
- [Vosk 文档](https://alphacephei.com/vosk/)
