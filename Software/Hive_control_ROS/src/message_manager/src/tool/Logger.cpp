/**
 * @file Logger.cpp
 * @brief 日志管理类的实现
 */

#include "Logger.hpp"
#include <iostream>

namespace tool {

// 获取单例实例
Logger& Logger::getInstance() {
    static Logger instance;  // 静态局部变量，线程安全的单例
    return instance;
}

// 构造函数：初始化默认配置
Logger::Logger() 
    : minLevel_(LogLevel::INFO)      // 默认最低级别为INFO
    , consoleOutput_(true) {         // 默认启用控制台输出
}

// 析构函数：确保日志文件正确关闭
Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

// 设置最低日志级别
void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);  // 加锁保证线程安全
    minLevel_ = level;
}

// 设置日志文件
void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);  // 加锁保证线程安全
    
    // 如果已有文件打开，先关闭
    if (logFile_.is_open()) {
        logFile_.close();
    }
    
    // 以追加模式打开新文件
    logFile_.open(filename, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

// 启用或禁用控制台输出
void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);  // 加锁保证线程安全
    consoleOutput_ = enable;
}

// 获取当前时间字符串，精确到毫秒
std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    // 计算毫秒部分
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // 格式化时间字符串：YYYY-MM-DD HH:MM:SS.mmm
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// 将日志级别转换为字符串
std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

// 核心日志记录函数
void Logger::log(LogLevel level, const std::string& message) {
    // 如果日志级别低于最低级别，直接返回
    if (level < minLevel_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);  // 加锁保证线程安全
    
    // 构建日志消息：[时间] [级别] 消息内容
    std::stringstream ss;
    ss << "[" << getCurrentTime() << "] "
       << "[" << levelToString(level) << "] "
       << message;
    
    std::string logMessage = ss.str();
    
    // 输出到控制台
    if (consoleOutput_) {
        // ERROR及以上级别输出到stderr，其他输出到stdout
        if (level >= LogLevel::ERROR) {
            std::cerr << logMessage << std::endl;
        } else {
            std::cout << logMessage << std::endl;
        }
    }
    
    // 输出到文件
    if (logFile_.is_open()) {
        logFile_ << logMessage << std::endl;
        logFile_.flush();  // 立即刷新到磁盘，防止丢失
    }
}

// 以下是各级别日志的便捷接口

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

} // namespace tool
