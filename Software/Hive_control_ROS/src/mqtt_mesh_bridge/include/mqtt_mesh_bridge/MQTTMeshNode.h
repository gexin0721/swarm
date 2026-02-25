#ifndef MQTT_MESH_NODE_H
#define MQTT_MESH_NODE_H

#include <mosquitto.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <functional>

/**
 * @brief MQTT Mesh节点 - 每个无人机既是Broker也是Client
 * 
 * 核心功能：
 * 1. 作为MQTT Broker接受子节点连接
 * 2. 作为MQTT Client连接到父节点
 * 3. 自动发现邻居节点并选择最优父节点
 * 4. 转发子节点数据到根节点
 * 5. 动态调整网络拓扑
 */
class MQTTMeshNode {
public:
    enum class NodeRole {
        ROOT,      // 根节点（上位机）
        RELAY,     // 中继节点（有子节点的无人机）
        LEAF       // 叶子节点（无