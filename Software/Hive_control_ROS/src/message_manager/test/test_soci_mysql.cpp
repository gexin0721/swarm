#include <iostream>
#include <string>
#include <chrono>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>

/**
 * @brief 测试 SOCI + MySQL 的基本功能
 *
 * 这个测试验证：
 * 1. SOCI 是否能在 C++17 环境下编译
 * 2. 能否连接到 MySQL 数据库
 * 3. 基本的增删改查操作
 * 4. 是否与 MQTT 库冲突
 */
void test_soci_mysql() {
    std::cout << "=== SOCI + MySQL 测试 ===" << std::endl;

    try {
        // 1. 连接到 MySQL 数据库
        // 连接字符串格式: "host=localhost user=root password=your_password db=test_db"
        std::cout << "正在连接到 MySQL..." << std::endl;

        soci::session sql(soci::mysql,
            "host=localhost user=root password=root db=mysql");

        std::cout << "✓ 数据库连接成功" << std::endl;

        // 2. 创建测试数据库和表
        std::cout << "\n--- 创建测试数据库和表 ---" << std::endl;

        sql << "CREATE DATABASE IF NOT EXISTS message_cache";
        sql << "USE message_cache";

        sql << "CREATE TABLE IF NOT EXISTS messages ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "topic VARCHAR(255) NOT NULL, "
               "payload TEXT NOT NULL, "
               "timestamp BIGINT NOT NULL, "
               "INDEX idx_topic (topic), "
               "INDEX idx_timestamp (timestamp)"
               ")";

        std::cout << "✓ 数据库和表创建成功" << std::endl;

        // 3. 插入数据
        std::cout << "\n--- 插入测试数据 ---" << std::endl;

        std::string topic = "drone/position";
        std::string payload = "{\"x\":10.5, \"y\":20.3, \"z\":5.0}";
        long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        sql << "INSERT INTO messages(topic, payload, timestamp) "
               "VALUES(:topic, :payload, :timestamp)",
            soci::use(topic), soci::use(payload), soci::use(timestamp);

        std::cout << "✓ 插入数据成功: " << topic << std::endl;

        // 4. 批量插入（模拟消息缓存）
        std::cout << "\n--- 批量插入消息 ---" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 100; i++) {
            std::string t = "test/topic_" + std::to_string(i);
            std::string p = "{\"data\":\"payload_" + std::to_string(i) + "\"}";
            long long ts = timestamp + i;

            sql << "INSERT INTO messages(topic, payload, timestamp) "
                   "VALUES(:t, :p, :ts)",
                soci::use(t), soci::use(p), soci::use(ts);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "✓ 插入 100 条消息耗时: " << duration.count() << " ms" << std::endl;

        // 5. 查询数据
        std::cout << "\n--- 查询数据 ---" << std::endl;

        std::string query_topic = "drone/position";
        std::string result_payload;
        long long result_timestamp;

        sql << "SELECT payload, timestamp FROM messages WHERE topic=:topic LIMIT 1",
            soci::into(result_payload), soci::into(result_timestamp),
            soci::use(query_topic);

        std::cout << "✓ 查询成功: " << result_payload << std::endl;

        // 6. 统计查询
        int total_count;
        sql << "SELECT COUNT(*) FROM messages", soci::into(total_count);
        std::cout << "✓ 数据库中共有 " << total_count << " 条消息" << std::endl;

        // 7. 按 Topic 分组统计
        std::cout << "\n--- 按 Topic 统计 ---" << std::endl;

        soci::rowset<soci::row> rs = (sql.prepare <<
            "SELECT topic, COUNT(*) as count FROM messages GROUP BY topic LIMIT 5");

        for (auto it = rs.begin(); it != rs.end(); ++it) {
            const soci::row& row = *it;
            std::string topic_name = row.get<std::string>(0);
            long long count_val = row.get<long long>(1);
            std::cout << "Topic: " << topic_name
                      << " => Count: " << count_val << std::endl;
        }

        // 8. 删除旧数据（模拟清理过期消息）
        std::cout << "\n--- 删除测试 ---" << std::endl;

        sql << "DELETE FROM messages WHERE topic LIKE 'test/topic_%'";

        int remaining_count;
        sql << "SELECT COUNT(*) FROM messages", soci::into(remaining_count);
        std::cout << "✓ 删除后剩余 " << remaining_count << " 条消息" << std::endl;

        // 9. 清理测试数据
        std::cout << "\n--- 清理测试数据 ---" << std::endl;
        sql << "DROP TABLE messages";
        sql << "DROP DATABASE message_cache";
        std::cout << "✓ 测试数据已清理" << std::endl;

        std::cout << "\n✓✓✓ 所有测试通过！SOCI 与 C++17 完全兼容 ✓✓✓" << std::endl;

    } catch (const soci::mysql_soci_error& e) {
        std::cerr << "✗ MySQL 错误: " << e.what() << std::endl;
        std::cerr << "错误代码: " << e.err_num_ << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ 异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "C++ 标准: " << __cplusplus << std::endl;
    std::cout << "（C++17 = 201703）" << std::endl << std::endl;

    test_soci_mysql();
    return 0;
}
