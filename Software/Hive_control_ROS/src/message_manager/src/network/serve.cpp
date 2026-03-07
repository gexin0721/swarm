#include "serve.hpp"
#include <iostream>

/**
 * @brief 构造函数，初始化 MQTT 客户端
 * @param server_address MQTT 服务器地址（例如：tcp://localhost:1883）
 * @param client_id 客户端唯一标识符
 */
serve::serve(const std::string& server_address, const std::string& client_id)
    : client_(new mqtt::async_client(server_address, client_id, 100))  // 100 是 maxBufferedMessages
    , callback_(new Callback(this)) {
    // 设置回调处理器，用于处理连接丢失、消息到达等事件
    client_->set_callback(*callback_);
}

/**
 * @brief 析构函数，确保断开连接
 */
serve::~serve() {
    if (isConnected()) {
        disconnect();
    }
}

/**
 * @brief 连接到 MQTT 服务器
 * @param username 用户名（可选）
 * @param password 密码（可选）
 * @return 连接成功返回 true，失败返回 false
 */
bool serve::connect(const std::string& username, const std::string& password) {
    username_ = username;
    password_ = password;

    // 配置连接选项
    mqtt::connect_options conn_opts;
    conn_opts.set_keep_alive_interval(20);      // 设置心跳间隔为 20 秒
    conn_opts.set_clean_session(true);          // 清除会话状态
    conn_opts.set_automatic_reconnect(true);    // 启用自动重连

    // 如果提供了用户名，则设置认证信息
    if (!username.empty()) {
        conn_opts.set_user_name(username);
        conn_opts.set_password(password);
    }

    try {
        // 异步连接并等待完成
        auto token = client_->connect(conn_opts);
        token->wait();

        // 触发连接成功回调
        if (connection_callback_) {
            connection_callback_(true);
        }
        return true;
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT connect failed: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 断开与 MQTT 服务器的连接
 */
void serve::disconnect() {
    try {
        // 异步断开连接并等待完成
        auto token = client_->disconnect();
        token->wait();

        // 触发连接断开回调
        if (connection_callback_) {
            connection_callback_(false);
        }
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT disconnect failed: " << e.what() << std::endl;
    }
}

/**
 * @brief 发布消息到指定主题
 * @param topic 主题名称
 * @param payload 消息内容
 * @param qos 服务质量等级（0, 1, 2）
 * @return 发布成功返回 true，失败返回 false
 */
bool serve::publish(const std::string& topic, const std::string& payload, int qos) {
    try {
        // 创建消息并设置 QoS
        auto msg = mqtt::make_message(topic, payload);
        msg->set_qos(qos);
        // 发布消息并等待完成
        client_->publish(msg)->wait();
        return true;
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT publish failed: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 订阅指定主题
 * @param topic 主题名称（支持通配符 # 和 +）
 * @param qos 服务质量等级（0, 1, 2）
 * @return 订阅成功返回 true，失败返回 false
 */
bool serve::subscribe(const std::string& topic, int qos) {
    try {
        client_->subscribe(topic, qos)->wait();
        return true;
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT subscribe failed: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 取消订阅指定主题
 * @param topic 主题名称
 * @return 取消订阅成功返回 true，失败返回 false
 */
bool serve::unsubscribe(const std::string& topic) {
    try {
        client_->unsubscribe(topic)->wait();
        return true;
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT unsubscribe failed: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 设置消息到达时的回调函数
 * @param callback 回调函数，参数为 (topic, payload)
 */
void serve::setMessageCallback(MessageCallback callback) {
    message_callback_ = callback;
}

/**
 * @brief 设置连接状态变化时的回调函数
 * @param callback 回调函数，参数为连接状态（true=已连接，false=已断开）
 */
void serve::setConnectionCallback(ConnectionCallback callback) {
    connection_callback_ = callback;
}

/**
 * @brief 检查是否已连接到 MQTT 服务器
 * @return 已连接返回 true，未连接返回 false
 */
bool serve::isConnected() const {
    return client_->is_connected();
}

// ============================================================================
// Callback 类实现 - 处理 MQTT 客户端的各种事件
// ============================================================================

/**
 * @brief 连接丢失时的回调处理
 * @param cause 连接丢失的原因
 */
void serve::Callback::connection_lost(const std::string& cause) {
    std::cerr << "MQTT connection lost: " << cause << std::endl;
    // 触发用户设置的连接状态回调
    if (client_->connection_callback_) {
        client_->connection_callback_(false);
    }
}

/**
 * @brief 消息到达时的回调处理
 * @param msg 接收到的消息对象
 */
void serve::Callback::message_arrived(mqtt::const_message_ptr msg) {
    // 触发用户设置的消息回调，传递主题和消息内容
    if (client_->message_callback_) {
        client_->message_callback_(msg->get_topic(), msg->to_string());
    }
}

/**
 * @brief 消息发送完成时的回调处理
 * @param token 发送令牌
 */
void serve::Callback::delivery_complete(mqtt::delivery_token_ptr token) {
    // 消息发送完成（可根据需要添加处理逻辑）
}
