#include "../src/network/serve.hpp"
#include "../src/tool/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

// 测试结果统计
struct TestResult {
    int total = 0;      // 总测试数
    int passed = 0;     // 通过数
    int failed = 0;     // 失败数
};

// 全局测试结果
TestResult g_test_result;

// 测试辅助宏
#define TEST_ASSERT(condition, test_name) \
    do { \
        g_test_result.total++; \
        if (condition) { \
            std::cout << "[PASS] " << test_name << std::endl; \
            g_test_result.passed++; \
        } else { \
            std::cout << "[FAIL] " << test_name << std::endl; \
            g_test_result.failed++; \
        } \
    } while(0)

/**
 * @brief 测试1: 基础连接测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_basic_connection(const std::string& broker_address) {
    std::cout << "\n=== 测试1: 基础连接测试 ===" << std::endl;

    try {
        // 创建客户端
        serve client(broker_address, "test_client_001");

        // 测试连接
        bool connected = client.connect();
        TEST_ASSERT(connected, "连接MQTT服务器");

        // 测试连接状态
        TEST_ASSERT(client.isConnected(), "检查连接状态");

        // 等待一下确保连接稳定
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 断开连接
        client.disconnect();
        TEST_ASSERT(!client.isConnected(), "断开连接后状态检查");

        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试2: 带认证的连接测试
 * @param broker_address MQTT服务器地址
 * @param username 用户名
 * @param password 密码
 * @return 测试是否通过
 */
bool test_authenticated_connection(const std::string& broker_address,
                                   const std::string& username,
                                   const std::string& password) {
    std::cout << "\n=== 测试2: 认证连接测试 ===" << std::endl;

    try {
        serve client(broker_address, "test_client_002");

        // 使用用户名密码连接
        bool connected = client.connect(username, password);
        TEST_ASSERT(connected, "使用认证信息连接");

        client.disconnect();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试3: 发布消息测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_publish_message(const std::string& broker_address) {
    std::cout << "\n=== 测试3: 发布消息测试 ===" << std::endl;

    try {
        serve client(broker_address, "test_client_003");

        if (!client.connect()) {
            TEST_ASSERT(false, "连接失败");
            return false;
        }

        // 测试不同QoS等级的发布
        bool pub_qos0 = client.publish("test/topic/qos0", "Hello QoS 0", 0);
        TEST_ASSERT(pub_qos0, "发布消息 QoS=0");

        bool pub_qos1 = client.publish("test/topic/qos1", "Hello QoS 1", 1);
        TEST_ASSERT(pub_qos1, "发布消息 QoS=1");

        bool pub_qos2 = client.publish("test/topic/qos2", "Hello QoS 2", 2);
        TEST_ASSERT(pub_qos2, "发布消息 QoS=2");

        // 测试发布JSON格式消息
        std::string json_msg = R"({"device":"test","value":123,"status":"ok"})";
        bool pub_json = client.publish("test/topic/json", json_msg, 1);
        TEST_ASSERT(pub_json, "发布JSON消息");

        client.disconnect();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试4: 订阅和接收消息测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_subscribe_and_receive(const std::string& broker_address) {
    std::cout << "\n=== 测试4: 订阅和接收消息测试 ===" << std::endl;

    try {
        serve client(broker_address, "test_client_004");

        if (!client.connect()) {
            TEST_ASSERT(false, "连接失败");
            return false;
        }

        // 用于记录接收到的消息
        std::atomic<int> message_count(0);
        std::string received_topic;
        std::string received_payload;

        // 设置消息回调
        client.setMessageCallback([&](const std::string& topic, const std::string& payload) {
            std::cout << "收到消息 - 主题: " << topic << ", 内容: " << payload << std::endl;
            received_topic = topic;
            received_payload = payload;
            message_count++;
        });

        // 订阅主题
        bool subscribed = client.subscribe("test/subscribe/topic", 1);
        TEST_ASSERT(subscribed, "订阅主题");

        // 等待订阅生效
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 发布消息到订阅的主题
        std::string test_message = "Test Subscribe Message";
        client.publish("test/subscribe/topic", test_message, 1);

        // 等待消息接收
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 验证消息接收
        TEST_ASSERT(message_count > 0, "接收到消息");
        TEST_ASSERT(received_topic == "test/subscribe/topic", "主题匹配");
        TEST_ASSERT(received_payload == test_message, "消息内容匹配");

        // 取消订阅
        bool unsubscribed = client.unsubscribe("test/subscribe/topic");
        TEST_ASSERT(unsubscribed, "取消订阅");

        client.disconnect();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试5: 通配符订阅测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_wildcard_subscribe(const std::string& broker_address) {
    std::cout << "\n=== 测试5: 通配符订阅测试 ===" << std::endl;

    try {
        serve client(broker_address, "test_client_005");

        if (!client.connect()) {
            TEST_ASSERT(false, "连接失败");
            return false;
        }

        std::atomic<int> message_count(0);

        client.setMessageCallback([&](const std::string& topic, const std::string& payload) {
            std::cout << "收到消息 - 主题: " << topic << ", 内容: " << payload << std::endl;
            message_count++;
        });

        // 测试 # 通配符 (匹配多级)
        client.subscribe("test/wildcard/#", 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 发布多条消息
        client.publish("test/wildcard/level1", "Message 1", 1);
        client.publish("test/wildcard/level1/level2", "Message 2", 1);
        client.publish("test/wildcard/level1/level2/level3", "Message 3", 1);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        TEST_ASSERT(message_count >= 3, "通配符#接收多级主题消息");

        client.disconnect();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试6: 连接回调测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_connection_callback(const std::string& broker_address) {
    std::cout << "\n=== 测试6: 连接回调测试 ===" << std::endl;

    try {
        serve client(broker_address, "test_client_006");

        std::atomic<bool> connect_callback_triggered(false);
        std::atomic<bool> disconnect_callback_triggered(false);

        // 设置连接状态回调
        client.setConnectionCallback([&](bool connected) {
            if (connected) {
                std::cout << "回调: 连接成功" << std::endl;
                connect_callback_triggered = true;
            } else {
                std::cout << "回调: 连接断开" << std::endl;
                disconnect_callback_triggered = true;
            }
        });

        // 连接
        client.connect();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        TEST_ASSERT(connect_callback_triggered, "连接回调触发");

        // 断开
        client.disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        TEST_ASSERT(disconnect_callback_triggered, "断开回调触发");

        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 测试7: 多客户端通信测试
 * @param broker_address MQTT服务器地址
 * @return 测试是否通过
 */
bool test_multi_client_communication(const std::string& broker_address) {
    std::cout << "\n=== 测试7: 多客户端通信测试 ===" << std::endl;

    try {
        // 创建发布者和订阅者
        serve publisher(broker_address, "test_publisher");
        serve subscriber(broker_address, "test_subscriber");

        if (!publisher.connect() || !subscriber.connect()) {
            TEST_ASSERT(false, "客户端连接失败");
            return false;
        }

        std::atomic<bool> message_received(false);

        // 订阅者设置回调
        subscriber.setMessageCallback([&](const std::string& topic, const std::string& payload) {
            std::cout << "订阅者收到: " << payload << std::endl;
            if (payload == "Multi-Client Test") {
                message_received = true;
            }
        });

        // 订阅主题
        subscriber.subscribe("test/multi/client", 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 发布者发送消息
        publisher.publish("test/multi/client", "Multi-Client Test", 1);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        TEST_ASSERT(message_received, "多客户端消息传递");

        publisher.disconnect();
        subscriber.disconnect();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 运行所有测试
 * @param broker_address MQTT服务器地址 (例如: "tcp://localhost:1883")
 * @param username 用户名 (可选)
 * @param password 密码 (可选)
 */
void run_all_tests(const std::string& broker_address,
                   const std::string& username = "",
                   const std::string& password = "") {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "开始MQTT客户端测试" << std::endl;
    std::cout << "Broker地址: " << broker_address << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // 重置测试结果
    g_test_result = TestResult();

    // 运行各项测试
    test_basic_connection(broker_address);

    if (!username.empty()) {
        test_authenticated_connection(broker_address, username, password);
    }

    test_publish_message(broker_address);
    test_subscribe_and_receive(broker_address);
    test_wildcard_subscribe(broker_address);
    test_connection_callback(broker_address);
    test_multi_client_communication(broker_address);

    // 输出测试总结
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "测试总结" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "总测试数: " << g_test_result.total << std::endl;
    std::cout << "通过: " << g_test_result.passed << " ("
              << (g_test_result.total > 0 ? (g_test_result.passed * 100 / g_test_result.total) : 0)
              << "%)" << std::endl;
    std::cout << "失败: " << g_test_result.failed << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    if (g_test_result.failed == 0) {
        std::cout << "✓ 所有测试通过!" << std::endl;
    } else {
        std::cout << "✗ 有测试失败,请检查!" << std::endl;
    }
}
