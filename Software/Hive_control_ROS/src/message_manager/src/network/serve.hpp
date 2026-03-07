#ifndef __serve_H_
#define __serve_H_

#include <string>
#include <functional>
#include <memory>
#include <mqtt/async_client.h>

// 备注：这里的serve指的是核心通信节点
class serve {
public:
    using MessageCallback = std::function<void(const std::string& topic, const std::string& payload)>;
    using ConnectionCallback = std::function<void(bool connected)>;

    serve(const std::string& server_address, const std::string& client_id);
    ~serve();

    // 连接到 MQTT 服务器
    bool connect(const std::string& username = "", const std::string& password = "");

    // 断开连接
    void disconnect();

    // 发布消息
    bool publish(const std::string& topic, const std::string& payload, int qos = 1);

    // 订阅主题
    bool subscribe(const std::string& topic, int qos = 1);

    // 取消订阅
    bool unsubscribe(const std::string& topic);

    // 设置消息回调
    void setMessageCallback(MessageCallback callback);

    // 设置连接状态回调
    void setConnectionCallback(ConnectionCallback callback);

    // 检查连接状态
    bool isConnected() const;

private:
    class Callback : public virtual mqtt::callback {
    public:
        Callback(serve* client) : client_(client) {}

        void connection_lost(const std::string& cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;

    private:
        serve* client_;
    };

    std::unique_ptr<mqtt::async_client> client_;
    std::unique_ptr<Callback> callback_;
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
    std::string username_;
    std::string password_;
};


#endif