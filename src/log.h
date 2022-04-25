/**
 * @file log.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-10 <th>1.0 <th>zhcs    <th>create
 * </table>
 */
#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>

#include "util.h"
#include "singleton.h"
#include "mutex.h"
#include "thread.h"

//static std::string thread_name("name");
// std::string& GetThreadName() { 
//     static std::string thread_name("name");
//     return thread_name;
// }

#define ZCS_LEVEL(logger, level) zcs::LogEventWrap(zcs::LogEvent::ptr(new zcs::LogEvent( \
                    logger, level, __FILE__, __LINE__, 0, zcs::GetThreadId(), zcs::GetFiberId(),\
                     time(0), zcs::Thread::GetCurName()))).GetEvent()->GetSS()

#define ZCS_DEBUG(logger) ZCS_LEVEL(logger, zcs::LogLevel::DEBUG)
#define ZCS_INFO(logger) ZCS_LEVEL(logger, zcs::LogLevel::INFO)
#define ZCS_WARN(logger) ZCS_LEVEL(logger, zcs::LogLevel::WARN)
#define ZCS_ERROR(logger) ZCS_LEVEL(logger, zcs::LogLevel::ERROR)
#define ZCS_FATAL(logger) ZCS_LEVEL(logger, zcs::LogLevel::FATAL)

#define ZCS_FMT_LEVEL(logger, level, format, ...) zcs::LogEventWrap(zcs::LogEvent::ptr(new zcs::LogEvent( \
                    logger, zcs::LogLevel::Level::DEBUG, __FILE__, __LINE__, 0, zcs::GetThreadId(),\
                     zcs::GetFiberId(), time(0), zcs::Thread::GetCurName()))).GetEvent()->Format(format, ##__VA_ARGS__)

#define ZCS_DEBUG_FMT(logger, format, ...) ZCS_FMT_LEVEL(logger, zcs::LogLevel::DEBUG, format, ##__VA_ARGS__)
#define ZCS_INFO_FMT(logger, format, ...) ZCS_FMT_LEVEL(logger, zcs::LogLevel::INFO, format, ##__VA_ARGS__)
#define ZCS_WARN_FMT(logger, format, ...) ZCS_FMT_LEVEL(logger, zcs::LogLevel::WARN, format, ##__VA_ARGS__)
#define ZCS_ERROR_FMT(logger, format, ...) ZCS_FMT_LEVEL(logger, zcs::LogLevel::ERROR, format, ##__VA_ARGS__)
#define ZCS_FATAL_FMT(logger, format, ...) ZCS_FMT_LEVEL(logger, zcs::LogLevel::FATAL, format, ##__VA_ARGS__)


#define ZCS_LOG_ROOT() zcs::LoggerMgr::GetInstance()->GetRoot()
#define ZCS_LOG_NAME(name) zcs::LoggerMgr::GetInstance()->GetLogger(name)

namespace zcs {

class Logger;

class LogLevel {
public:
    enum Level {
        UNKONW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& level);
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file
            ,int32_t line, uint32_t elapse, uint32_t thread_id
            ,uint32_t fiber_id, uint64_t time, const std::string& thread_name);
    
    const char* GetFile() const { return file_;}
    int32_t GetLine() const { return line_;}
    uint32_t GetElapse() const { return elapse_;}
    uint32_t GetThreadId() const { return thread_id_;}
    uint32_t GetFiberId() const { return fiber_id_;}
    uint64_t GetTime() const { return time_;}
    std::shared_ptr<Logger> GetLogger() const { return logger_;}
    LogLevel::Level GetLevel() const { return level_;}
    std::stringstream& GetSS() { return ss_;}
    std::string GetContent() const { return ss_.str();}
    const std::string& GetThreadName() const { return thread_name_;}

    void Format(const char* format, ...);
    void Format(const char* format, va_list vl);
private:
    const char* file_;
    int32_t line_;
    uint32_t elapse_;
    uint32_t thread_id_;
    uint32_t fiber_id_;
    uint64_t time_;
    std::string thread_name_;
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_;
    std::stringstream ss_;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    /**
     * @brief Construct a new Log Formatter object
     * 
     * @param [in] pattern 
     * 
     * @details 
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     */
    LogFormatter(const std::string& pattern);
    std::string Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& Format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    void Init();
    bool IsError() const { return error_;}
    const std::string GetPattern() const { return pattern_;}

public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}

        virtual void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

private:
    std::string pattern_;
    bool error_ = false;
    std::vector<FormatItem::ptr> items_;
};

class LogAppender {
friend class Logger;
public:
    typedef Mutex MutexType;
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void SetFormatter(LogFormatter::ptr formatter);
    LogFormatter::ptr GetFormatter();
    void SetLevel(LogLevel::Level level) { level_ = level;};
    LogLevel::Level GetLevel() const { return level_;};

    virtual std::string ToYamlString() = 0;
protected:
    MutexType mutex_;
    LogLevel::Level level_ = LogLevel::DEBUG;
    bool has_formatter_ = false;
    LogFormatter::ptr formatter_;
};

class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Mutex MutexType;
    // static Logger* GetInstance() {
    //     static Logger instance;
    //     return &instance;
    // }

    Logger(const std::string& name = "root");
    ~Logger();
    void Log(LogLevel::Level level, LogEvent::ptr event);

    void AddAppender(LogAppender::ptr appender);
    void DelAppender(LogAppender::ptr appender);
    void ClearAppender();

    void SetFormatter(const std::string& format);
    void SetFormatter(LogFormatter::ptr format);
    LogFormatter::ptr GetFormatter();
    
    void SetLevel(LogLevel::Level level) { level_ = level;}
    LogLevel::Level GetLevel() const { return level_;}

    std::string GetName() const { return name_;}

    std::string ToYamlString();
private:
    MutexType mutex_;
    std::string name_;
    LogLevel::Level level_;
    std::list<LogAppender::ptr> appenders_;
    Logger::ptr root_;
    LogFormatter::ptr formatter_;
};





class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string ToYamlString() override;
private:
};

class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    bool Reopen();

    std::string ToYamlString() override;
private:
    std::string filename_;
    std::ofstream filestream_;
    uint64_t last_time_ = 0;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr event);
    ~LogEventWrap();
    std::shared_ptr<LogEvent> GetEvent() const { return event_;}
private:
    LogEvent::ptr event_;
};

class LoggerManager {
public:
    typedef Mutex MutexType;
    LoggerManager();

    Logger::ptr GetLogger(const std::string& name);
    Logger::ptr GetRoot() const { return root_;}
    void Init();

    std::string ToYamlString();
private:
    MutexType mutex_;
    Logger::ptr root_;
    std::map<std::string, Logger::ptr> loggers_;
};

typedef Singleton<LoggerManager> LoggerMgr;

} // namespace zcs



#endif //__LOG_H__