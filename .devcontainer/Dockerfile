FROM osrf/ros:noetic-desktop-full

# 设置非交互式安装
ENV DEBIAN_FRONTEND=noninteractive
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# 设置工作目录
WORKDIR /workspace

# ============================================
# 第一部分：系统基础工具
# ============================================
RUN apt-get update && apt-get install -y \
    # 基础开发工具
    build-essential \
    cmake \
    git \
    wget \
    curl \
    vim \
    nano \
    sudo \
    # Python 环境
    python3-pip \
    python3-dev \
    # 网络工具
    net-tools \
    iputils-ping \
    iproute2 \
    # 其他工具
    htop \
    tree \
    zip \
    unzip \
    ca-certificates \
    terminator \
    && rm -rf /var/lib/apt/lists/*

# ============================================
# 第二部分：robot.sh 中的系统依赖
# ============================================
RUN apt-get update && apt-get install -y \
    # 音频相关
    libasound2-dev \
    sox \
    libsox-fmt-all \
    libportaudio2 \
    portaudio19-dev \
    # 其他依赖
    nfs-common \
    && rm -rf /var/lib/apt/lists/*

# ============================================
# 第三部分：ROS 无人机集群相关包
# ============================================
RUN apt-get update && apt-get install -y \
    # ROS 基础包
    ros-noetic-amcl \
    ros-noetic-controller-manager \
    ros-noetic-dynamic-reconfigure \
    # Gazebo 仿真包
    ros-noetic-gazebo-plugins \
    ros-noetic-gazebo-ros \
    ros-noetic-gazebo-ros-control \
    # SLAM 和几何包
    ros-noetic-geometry-msgs \
    ros-noetic-gmapping \
    ros-noetic-hector-slam \
    # 导航基础包
    ros-noetic-joint-state-publisher \
    ros-noetic-map-server \
    ros-noetic-move-base \
    # MAVLink 通信包
    ros-noetic-mavros \
    ros-noetic-mavros-msgs \
    ros-noetic-mavros-extras \
    # 消息和状态包
    ros-noetic-nav-msgs \
    ros-noetic-robot-state-publisher \
    ros-noetic-rosbag \
    # ROS 核心包
    ros-noetic-roscpp \
    ros-noetic-roslaunch \
    ros-noetic-rospy \
    ros-noetic-rviz \
    # 传感器和变换包
    ros-noetic-sensor-msgs \
    ros-noetic-std-msgs \
    ros-noetic-tf2 \
    ros-noetic-tf2-geometry-msgs \
    ros-noetic-tf2-ros \
    # 机器人描述包
    ros-noetic-urdf \
    ros-noetic-visualization-msgs \
    ros-noetic-xacro \
    # 导航扩展包
    ros-noetic-navigation \
    ros-noetic-robot-localization \
    && rm -rf /var/lib/apt/lists/*

# ============================================
# 第四部分：Python 依赖包
# ============================================
RUN pip3 install --no-cache-dir \
    websocket-client \
    vosk \
    sounddevice \
    numpy \
    pandas \
    matplotlib

# ============================================
# 第五部分：cJSON 库（从源码编译）
# ============================================
RUN cd /tmp && \
    git clone --depth 1 --branch v1.7.17 https://github.com/DaveGamble/cJSON.git && \
    cd cJSON && \
    mkdir build && cd build && \
    cmake -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd / && rm -rf /tmp/cJSON

# ============================================
# 第六部分：ODB ORM 库
# ============================================
RUN apt-get update && apt-get install -y \
    # ODB 编译器和库
    odb \
    libodb-dev \
    libodb-sqlite-dev \
    libodb-boost-dev \
    # SQLite 开发库
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# ============================================
# 第七部分：EMQX 5.8 MQTT Broker
# ============================================
# 下载并安装 EMQX 5.8
RUN cd /tmp && \
    wget https://www.emqx.com/en/downloads/broker/5.8.0/emqx-5.8.0-ubuntu20.04-amd64.deb && \
    apt-get update && \
    apt-get install -y ./emqx-5.8.0-ubuntu20.04-amd64.deb && \
    rm emqx-5.8.0-ubuntu20.04-amd64.deb && \
    rm -rf /var/lib/apt/lists/*

# ============================================
# 第八部分：配置动态链接库
# ============================================
RUN echo "/usr/local/lib" >> /etc/ld.so.conf && \
    ldconfig

# ============================================
# 第九部分：ROS 环境配置
# ============================================
RUN echo "source /opt/ros/noetic/setup.bash" >> /root/.bashrc && \
    echo "export ROS_HOSTNAME=localhost" >> /root/.bashrc && \
    echo "export ROS_MASTER_URI=http://localhost:11311" >> /root/.bashrc

# ============================================
# 第十部分：创建工作目录结构
# ============================================
RUN mkdir -p /workspace/moxi /workspace/chenxu

# 设置默认 shell
CMD ["/bin/bash"]
