#include "SnowflakeIdGenerator.h"

SnowflakeIdGenerator::SnowflakeIdGenerator(uint16_t datacenter_id, uint16_t worker_id)
    : datacenter_id_(datacenter_id)
    , worker_id_(worker_id)
    , sequence_(0)
    , last_timestamp_(-1) {
    
    if (datacenter_id > MAX_DATACENTER_ID) {
        throw std::invalid_argument("Datacenter ID must be between 0 and " + std::to_string(MAX_DATACENTER_ID));
    }
    if (worker_id > MAX_WORKER_ID) {
        throw std::invalid_argument("Worker ID must be between 0 and " + std::to_string(MAX_WORKER_ID));
    }
}

int64_t SnowflakeIdGenerator::nextId() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    int64_t timestamp = getCurrentTimestamp();
    
    // 时钟回拨检测
    if (timestamp < last_timestamp_) {
        throw std::runtime_error("Clock moved backwards. Refusing to generate id for " + 
                               std::to_string(last_timestamp_ - timestamp) + " milliseconds");
    }
    
    // 同一毫秒内
    if (timestamp == last_timestamp_) {
        sequence_ = (sequence_ + 1) & MAX_SEQUENCE;
        // 序列号溢出，等待下一毫秒
        if (sequence_ == 0) {
            timestamp = waitNextMillis(last_timestamp_);
        }
    } else {
        sequence_ = 0;
    }
    
    last_timestamp_ = timestamp;
    
    // 组装64位ID
    int64_t id = ((timestamp - EPOCH) << TIMESTAMP_SHIFT) |
                 (datacenter_id_ << DATACENTER_SHIFT) |
                 (worker_id_ << WORKER_SHIFT) |
                 sequence_;
    
    return id;
}

void SnowflakeIdGenerator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    sequence_ = 0;
    last_timestamp_ = -1;
}

int64_t SnowflakeIdGenerator::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return duration.count();
}

int64_t SnowflakeIdGenerator::waitNextMillis(int64_t last_timestamp) const {
    int64_t timestamp = getCurrentTimestamp();
    while (timestamp <= last_timestamp) {
        timestamp = getCurrentTimestamp();
    }
    return timestamp;
}
