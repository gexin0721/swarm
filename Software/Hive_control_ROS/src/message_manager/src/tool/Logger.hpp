/**
 * @file Logger.hpp
 * @brief 线程安全的日志管理类
 * @author DroneSwarm Team
 * @date 2026
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace tool {

/**
 * @brief 日志级别枚举
 * 从低到高依次为：DEBUG < INFO < WARNING < ERROR < FATAL
 */
enum class LogLevel {
    DEBUG,      // 调试信息
    INFO,       // 一般信息
    WARNING,    // 警告信息
    ERROR,      // 错误信息
    FATAL       // 致命错误
};

/**
 * @brief 单例模式的日志管理类
 * 
 * 功能特性：
 * - 线程安全的日志记录
 * - 支持多种日志级别
 * - 可同时输出到控制台和文件
 * - 自动添加时间戳
 * 
 * 使用示例：
 * @code
 * Logger& logger = Logger::getInstance();
 * logger.setLogFile("app.log");
 * logger.info("程序启动");
 * logger.error("发生错误");
 * @endcode
 */
class Logger {
public:
    /**
     * @brief 获取日志管理器的单例实例
     * @return Logger的唯一实例引用
     */
    static Logger& getInstance();
    
    /**
     * @brief 设置最低日志级别
     * @param level 只有大于等于此级别的日志才会被记录
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief 设置日志文件路径
     * @param filename 日志文件的完整路径，以追加模式打开
     * @note 如果不设置，默认保存到 ./logs/message_manager.log
     */
    void setLogFile(const std::string& filename);
    
    /**
     * @brief 初始化默认日志文件
     * @param logDir 日志目录路径，默认为 "./logs"
     * @param logName 日志文件名，默认为 "message_manager.log"
     * @return 是否成功创建日志文件
     */
    bool initDefaultLogFile(const std::string& logDir = "./logs", 
                           const std::string& logName = "message_manager.log");
    
    /**
     * @brief 启用或禁用控制台输出
     * @param enable true为启用，false为禁用
     */
    void enableConsoleOutput(bool enable);
    
    /**
     * @brief 记录指定级别的日志
     * @param level 日志级别
     * @param message 日志内容
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief 记录DEBUG级别日志
     * @param message 日志内容
     */
    void debug(const std::string& message);
    
    /**
     * @brief 记录INFO级别日志
     * @param message 日志内容
     */
    void info(const std::string& message);
    
    /**
     * @brief 记录WARNING级别日志
     * @param message 日志内容
     */
    void warning(const std::string& message);
    
    /**
     * @brief 记录ERROR级别日志
     * @param message 日志内容
     */
    void error(const std::string& message);
    
    /**
     * @brief 记录FATAL级别日志
     * @param message 日志内容
     */
    void fatal(const std::string& message);
    
private:
    /**
     * @brief 私有构造函数，实现单例模式
     */
    Logger();
    
    /**
     * @brief 析构函数，关闭日志文件
     */
    ~Logger();
    
    // 禁止拷贝构造和赋值操作
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief 获取当前时间字符串
     * @return 格式化的时间字符串 (YYYY-MM-DD HH:MM:SS.mmm)
     */
    std::string getCurrentTime();
    
    /**
     * @brief 将日志级别转换为字符串
     * @param level 日志级别
     * @return 日志级别对应的字符串
     */
    std::string levelToString(LogLevel level);
    
    LogLevel minLevel_;         ///< 最低日志级别
    bool consoleOutput_;        ///< 是否输出到控制台
    std::ofstream logFile_;     ///< 日志文件流
    std::mutex mutex_;          ///< 互斥锁，保证线程安全
};

} // namespace tool

#endif // LOGGER_HPP
