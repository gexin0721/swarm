#include <iostream>
#include <string>
#include <chrono>
#include <leveldb/db.h>

/**
 * @brief 测试 LevelDB 的基本功能
 *
 * 这个测试函数验证：
 * 1. 数据库的创建和打开
 * 2. 数据的写入（Put）
 * 3. 数据的读取（Get）
 * 4. 数据的删除（Delete）
 * 5. 性能测试（批量写入）
 */
void test_leveldb_basic() {
    std::cout << "=== LevelDB 基础功能测试 ===" << std::endl;

    // 1. 打开数据库（如果不存在则创建）
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;  // 数据库不存在时自动创建

    leveldb::Status status = leveldb::DB::Open(options, "/tmp/test_leveldb", &db);

    if (!status.ok()) {
        std::cerr << "无法打开数据库: " << status.ToString() << std::endl;
        return;
    }
    std::cout << "✓ 数据库打开成功" << std::endl;

    // 2. 写入数据
    std::string key1 = "drone/position";
    std::string value1 = "{\"x\":10.5, \"y\":20.3, \"z\":5.0}";

    status = db->Put(leveldb::WriteOptions(), key1, value1);
    if (status.ok()) {
        std::cout << "✓ 写入数据成功: " << key1 << std::endl;
    } else {
        std::cerr << "✗ 写入失败: " << status.ToString() << std::endl;
    }

    // 3. 读取数据
    std::string read_value;
    status = db->Get(leveldb::ReadOptions(), key1, &read_value);
    if (status.ok()) {
        std::cout << "✓ 读取数据成功: " << read_value << std::endl;
    } else {
        std::cerr << "✗ 读取失败: " << status.ToString() << std::endl;
    }

    // 4. 写入多条消息（模拟消息缓存）
    std::cout << "\n--- 模拟消息缓存 ---" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::string key = "message/" + std::to_string(i);
        std::string value = "{\"topic\":\"test\", \"payload\":\"data_" + std::to_string(i) + "\"}";
        db->Put(leveldb::WriteOptions(), key, value);
        std::cout << "✓ 缓存消息 " << i << std::endl;
    }

    // 5. 遍历所有数据
    std::cout << "\n--- 遍历所有缓存的消息 ---" << std::endl;
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    int count = 0;
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::cout << "Key: " << it->key().ToString()
                  << " => Value: " << it->value().ToString() << std::endl;
        count++;
    }
    std::cout << "总共 " << count << " 条记录" << std::endl;
    delete it;

    // 6. 删除数据
    std::cout << "\n--- 删除测试 ---" << std::endl;
    status = db->Delete(leveldb::WriteOptions(), "message/0");
    if (status.ok()) {
        std::cout << "✓ 删除 message/0 成功" << std::endl;
    }

    // 7. 性能测试（批量写入）
    std::cout << "\n--- 性能测试：写入 1000 条消息 ---" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++) {
        std::string key = "perf_test/" + std::to_string(i);
        std::string value = "{\"timestamp\":" + std::to_string(i) + ", \"data\":\"test_payload\"}";
        db->Put(leveldb::WriteOptions(), key, value);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "✓ 写入 1000 条消息耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "✓ 平均每条消息: " << duration.count() / 1000.0 << " ms" << std::endl;

    // 8. 清理
    delete db;
    std::cout << "\n✓ 测试完成，数据库已关闭" << std::endl;
}

int main() {
    test_leveldb_basic();
    return 0;
}
