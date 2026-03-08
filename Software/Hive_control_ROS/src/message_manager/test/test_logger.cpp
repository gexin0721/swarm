/**
 * @file test_logger.cpp
 * @brief Logger模块的单元测试
 * @details 测试Logger类的各项功能，包括：
 *          - 单例模式验证
 *          - 日志级别过滤
 *          - 文件输出功能
 *          - 控制台输出开关
 *          - 多级别日志记录
 *          - 目录自动创建
 *          - 线程安全性
 */

#include <gtest/gtest.h>
#include "../src/tool/Logger.hpp"
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <regex>

using namespace tool;
namespace fs = std::filesystem;

/**
 * @class LoggerTest
 * @brief Logger测试类的基类
 * @details 提供测试环境的初始化和清理功能
 */
class LoggerTest : public ::testing::Test {
protected:
    // 测试用的临时目录和文件
    const std::string testLogDir = "./test_logs";
    const std::string testLogFile = "./test_logs/test.log";

    /**
     * @brief 每个测试用例开始前执行
     * @details 创建测试用的临时目录
     */
    void SetUp() override {
        // 创建测试目录
        if (!fs::exists(testLogDir)) {
            fs::create_directories(testLogDir);
        }
    }

    /**
     * @brief 每个测试用例结束后执行
     * @details 清理测试产生的临时文件和目录
     */
    void TearDown() override {
        // 清理测试文件和目录（已注释，保留日志文件用于调试）
        // if (fs::exists(testLogDir)) {
        //     fs::remove_all(testLogDir);
        // }
    }

    /**
     * @brief 读取日志文件的全部内容
     * @param filename 日志文件路径
     * @return 文件内容字符串
     */
    std::string readLogFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    /**
     * @brief 统计日志文件中的行数
     * @param filename 日志文件路径
     * @return 行数
     */
    int countLogLines(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return 0;
        }
        int count = 0;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                count++;
            }
        }
        return count;
    }
};

/**
 * @test 测试单例模式
 * @details 验证多次调用getInstance()返回的是同一个实例
 */
TEST_F(LoggerTest, SingletonPattern) {
    Logger& logger1 = Logger::getInstance();
    Logger& logger2 = Logger::getInstance();

    // 验证两个引用指向同一个对象
    EXPECT_EQ(&logger1, &logger2);
}

/**
 * @test 测试基本的日志记录功能
 * @details 验证日志能够正确写入文件，并包含正确的格式
 */
TEST_F(LoggerTest, BasicLogging) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);  // 关闭控制台输出，避免干扰测试

    // 记录一条INFO日志
    logger.info("Test message");

    // 读取日志文件内容
    std::string content = readLogFile(testLogFile);

    // 验证日志内容包含关键信息
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
    EXPECT_TRUE(content.find("Test message") != std::string::npos);

    // 验证时间戳格式 [YYYY-MM-DD HH:MM:SS.mmm]
    std::regex timePattern(R"(\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\])");
    EXPECT_TRUE(std::regex_search(content, timePattern));
}

/**
 * @test 测试所有日志级别
 * @details 验证DEBUG、INFO、WARNING、ERROR、FATAL五个级别都能正常工作
 */
TEST_F(LoggerTest, AllLogLevels) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);
    logger.setLogLevel(LogLevel::DEBUG);  // 设置为最低级别，记录所有日志

    // 记录各个级别的日志
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.fatal("Fatal message");

    // 读取日志文件
    std::string content = readLogFile(testLogFile);

    // 验证所有级别都被记录
    EXPECT_TRUE(content.find("DEBUG") != std::string::npos);
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
    EXPECT_TRUE(content.find("WARNING") != std::string::npos);
    EXPECT_TRUE(content.find("ERROR") != std::string::npos);
    EXPECT_TRUE(content.find("FATAL") != std::string::npos);

    // 验证消息内容
    EXPECT_TRUE(content.find("Debug message") != std::string::npos);
    EXPECT_TRUE(content.find("Info message") != std::string::npos);
    EXPECT_TRUE(content.find("Warning message") != std::string::npos);
    EXPECT_TRUE(content.find("Error message") != std::string::npos);
    EXPECT_TRUE(content.find("Fatal message") != std::string::npos);
}

/**
 * @test 测试日志级别过滤功能
 * @details 验证设置最低级别后，低于该级别的日志不会被记录
 */
TEST_F(LoggerTest, LogLevelFiltering) {
    Logger& logger = Logger::getInstance();

    // 使用独立的测试文件
    std::string filterTestFile = "./test_logs/filter_test.log";
    logger.setLogFile(filterTestFile);
    logger.enableConsoleOutput(false);

    // 设置最低级别为WARNING
    logger.setLogLevel(LogLevel::WARNING);

    // 记录各个级别的日志
    logger.debug("Debug message");      // 不应该被记录
    logger.info("Info message");        // 不应该被记录
    logger.warning("Warning message");  // 应该被记录
    logger.error("Error message");      // 应该被记录
    logger.fatal("Fatal message");      // 应该被记录

    // 读取日志文件
    std::string content = readLogFile(filterTestFile);

    // 验证低级别日志没有被记录
    EXPECT_TRUE(content.find("DEBUG") == std::string::npos);
    EXPECT_TRUE(content.find("INFO") == std::string::npos);
    EXPECT_TRUE(content.find("Debug message") == std::string::npos);
    EXPECT_TRUE(content.find("Info message") == std::string::npos);

    // 验证高级别日志被记录
    EXPECT_TRUE(content.find("WARNING") != std::string::npos);
    EXPECT_TRUE(content.find("ERROR") != std::string::npos);
    EXPECT_TRUE(content.find("FATAL") != std::string::npos);
}

/**
 * @test 测试目录自动创建功能
 * @details 验证initDefaultLogFile能够自动创建不存在的目录
 */
TEST_F(LoggerTest, AutoCreateDirectory) {
    Logger& logger = Logger::getInstance();
    logger.enableConsoleOutput(false);

    // 使用一个不存在的目录
    std::string newLogDir = "./test_logs/auto_created";
    std::string newLogFile = "auto_test.log";

    // 确保目录不存在
    if (fs::exists(newLogDir)) {
        fs::remove_all(newLogDir);
    }

    // 初始化日志文件（应该自动创建目录）
    bool result = logger.initDefaultLogFile(newLogDir, newLogFile);

    // 验证目录被创建
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists(newLogDir));
    EXPECT_TRUE(fs::exists(newLogDir + "/" + newLogFile));

    // 清理
    if (fs::exists(newLogDir)) {
        fs::remove_all(newLogDir);
    }
}

/**
 * @test 测试日志文件追加模式
 * @details 验证多次写入日志时，内容是追加而不是覆盖
 */
TEST_F(LoggerTest, AppendMode) {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::INFO);  // 确保INFO级别日志会被记录
    logger.enableConsoleOutput(false);

    // 使用独立的测试文件
    std::string appendTestFile = "./test_logs/append_test.log";

    // 第一次写入
    logger.setLogFile(appendTestFile);
    logger.info("First message");

    // 强制刷新并关闭，然后重新打开同一个文件
    logger.setLogFile(appendTestFile);
    logger.info("Second message");

    // 等待文件写入完成
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // 读取文件内容
    std::string content = readLogFile(appendTestFile);

    // 验证两条消息都存在
    EXPECT_TRUE(content.find("First message") != std::string::npos)
        << "File content: " << content;
    EXPECT_TRUE(content.find("Second message") != std::string::npos)
        << "File content: " << content;
}

/**
 * @test 测试线程安全性
 * @details 验证多线程并发写入日志时不会出现数据竞争或崩溃
 */
TEST_F(LoggerTest, ThreadSafety) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);
    logger.setLogLevel(LogLevel::INFO);

    // 记录写入前的行数
    int linesBefore = countLogLines(testLogFile);

    const int numThreads = 10;      // 线程数量
    const int logsPerThread = 100;  // 每个线程写入的日志数量

    std::vector<std::thread> threads;

    // 创建多个线程并发写入日志
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&logger, i, logsPerThread]() {
            for (int j = 0; j < logsPerThread; ++j) {
                logger.info("Thread " + std::to_string(i) + " - Message " + std::to_string(j));
            }
        });
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 验证新增的日志行数是否正确
    int linesAfter = countLogLines(testLogFile);
    int newLines = linesAfter - linesBefore;
    EXPECT_EQ(newLines, numThreads * logsPerThread);
}

/**
 * @test 测试控制台输出开关
 * @details 验证enableConsoleOutput能够正确控制是否输出到控制台
 * @note 这个测试主要验证函数调用不会崩溃，实际控制台输出难以自动化测试
 */
TEST_F(LoggerTest, ConsoleOutputToggle) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);

    // 测试启用控制台输出
    logger.enableConsoleOutput(true);
    logger.info("Console enabled");

    // 测试禁用控制台输出
    logger.enableConsoleOutput(false);
    logger.info("Console disabled");

    // 验证日志文件中两条消息都存在
    std::string content = readLogFile(testLogFile);
    EXPECT_TRUE(content.find("Console enabled") != std::string::npos);
    EXPECT_TRUE(content.find("Console disabled") != std::string::npos);
}

/**
 * @test 测试空消息处理
 * @details 验证记录空字符串时不会崩溃
 */
TEST_F(LoggerTest, EmptyMessage) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);

    // 记录空消息
    logger.info("");

    // 验证不会崩溃，且文件中有记录
    int lineCount = countLogLines(testLogFile);
    EXPECT_GT(lineCount, 0);
}

/**
 * @test 测试长消息处理
 * @details 验证能够正确处理很长的日志消息
 */
TEST_F(LoggerTest, LongMessage) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);

    // 创建一个很长的消息（10000个字符）
    std::string longMessage(10000, 'A');
    logger.info(longMessage);

    // 验证消息被完整记录
    std::string content = readLogFile(testLogFile);
    EXPECT_TRUE(content.find(longMessage) != std::string::npos);
}

/**
 * @test 测试特殊字符处理
 * @details 验证日志能够正确处理包含特殊字符的消息
 */
TEST_F(LoggerTest, SpecialCharacters) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile(testLogFile);
    logger.enableConsoleOutput(false);

    // 包含各种特殊字符的消息
    std::string specialMsg = "Special chars: \n\t\"'\\!@#$%^&*()中文测试";
    logger.info(specialMsg);

    // 验证消息被记录（注意：换行符会被实际处理）
    std::string content = readLogFile(testLogFile);
    EXPECT_TRUE(content.find("Special chars") != std::string::npos);
    EXPECT_TRUE(content.find("中文测试") != std::string::npos);
}

/**
 * @brief 主函数：运行所有测试
 */
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
