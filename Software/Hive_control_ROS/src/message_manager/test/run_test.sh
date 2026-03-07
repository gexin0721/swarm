#!/bin/bash

# MQTT客户端测试脚本

# 设置ROS环境
source /opt/ros/noetic/setup.bash
source /workspace/Software/Hive_control_ROS/devel/setup.bash

# 默认参数
MQTT_BROKER="tcp://localhost:1883"
MQTT_USERNAME=""
MQTT_PASSWORD=""

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --broker)
            MQTT_BROKER="$2"
            shift 2
            ;;
        --username)
            MQTT_USERNAME="$2"
            shift 2
            ;;
        --password)
            MQTT_PASSWORD="$2"
            shift 2
            ;;
        *)
            echo "未知参数: $1"
            echo "用法: $0 [--broker tcp://localhost:1883] [--username user] [--password pass]"
            exit 1
            ;;
    esac
done

echo "=========================================="
echo "MQTT客户端测试"
echo "=========================================="
echo "Broker: $MQTT_BROKER"
if [ -n "$MQTT_USERNAME" ]; then
    echo "Username: $MQTT_USERNAME"
fi
echo "=========================================="
echo ""

# 检查MQTT Broker是否可用
echo "检查MQTT Broker连接..."
BROKER_HOST=$(echo $MQTT_BROKER | sed 's/tcp:\/\///' | cut -d':' -f1)
BROKER_PORT=$(echo $MQTT_BROKER | sed 's/tcp:\/\///' | cut -d':' -f2)

if [ -z "$BROKER_PORT" ]; then
    BROKER_PORT=1883
fi

timeout 2 bash -c "cat < /dev/null > /dev/tcp/$BROKER_HOST/$BROKER_PORT" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ MQTT Broker可访问"
else
    echo "✗ 警告: 无法连接到MQTT Broker ($BROKER_HOST:$BROKER_PORT)"
    echo "  请确保MQTT Broker正在运行"
    echo "  可以使用以下命令启动mosquitto:"
    echo "  mosquitto -v"
    echo ""
fi

# 运行测试
echo "开始运行测试..."
echo ""

if [ -n "$MQTT_USERNAME" ]; then
    rosrun message_manager message_manager \
        _run_tests:=true \
        _mqtt_broker:="$MQTT_BROKER" \
        _mqtt_username:="$MQTT_USERNAME" \
        _mqtt_password:="$MQTT_PASSWORD"
else
    rosrun message_manager message_manager \
        _run_tests:=true \
        _mqtt_broker:="$MQTT_BROKER"
fi
