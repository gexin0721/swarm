#ifndef SNOWFLAKE_ID_GENERATOR_H
#define SNOWFLAKE_ID_GENERATOR_H

#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <chrono>

/**
 * @brief 雪花算法ID生成器
 * 
 * 64位ID结构:
 * - 1位符号位(始终为0)
 * - 41位时间戳(毫秒级)
 * - 10位工作机器ID(5位数据中心ID + 5位机器ID)
 * - 12位序列号
 */
class SnowflakeIdGenerator {
public:
    /**
     * @brief 构造函数
     * @param datacenter_id 数据中心ID (0-31)
     * @param worker_id 工作机器ID (0-31)
     */
    SnowflakeIdGenerator(uint16_t datacenter_id = 0, uint16_t worker_id = 0);

    /**
     * @brief 生成唯一ID
     * @return 64位唯一ID
     */
    int64_t nextId();

    /**
     * @brief 重置生成器
     */
    void reset();

private:
    // 时间戳相关
    static constexpr int64_t EPOCH = 1609459200000LL; // 2021-01-01 00:00:00
    static constexpr int64_t TIMESTAMP_BITS = 41;
    static constexpr int64_t DATACENTER_BITS = 5;
    static constexpr int64_t WORKER_BITS = 5;
    static constexpr int64_t SEQUENCE_BITS = 12;

    // 最大值
    static constexpr int64_t MAX_DATACENTER_ID = (1 << DATACENTER_BITS) - 1;
    static constexpr int64_t MAX_WORKER_ID = (1 << WORKER_BITS) - 1;
    static constexpr int64_t MAX_SEQUENCE = (1 << SEQUENCE_BITS) - 1;

    // 位移量
    static constexpr int64_t WORKER_SHIFT = SEQUENCE_BITS;
    static constexpr int64_t DATACENTER_SHIFT = SEQUENCE_BITS + WORKER_BITS;
    static constexpr int64_t TIMESTAMP_SHIFT = SEQUENCE_BITS + WORKER_BITS + DATACENTER_BITS;

    uint16_t datacenter_id_;
    uint16_t worker_id_;
    int64_t sequence_;
    int64_t last_timestamp_;
    std::mutex mutex_;

    int64_t getCurrentTimestamp() const;
    int64_t waitNextMillis(int64_t last_timestamp) const;
};

#endif // SNOWFLAKE_ID_GENERATOR_H
