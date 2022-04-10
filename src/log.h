#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <memory>
#include <sstream>

#define LOG_DEBUG_FORMAT(logger, format, ...)  zcs::LogEventWrap(zcs::LogEvent::ptr(new zcs::LogEvent( \
                    logger, zcs::LogLevel::Level::DEBUG, __FILE__, __LINE__, 0, 1, 2, 3))).GetEvent()->Format(format, ##__VA_ARGS__)

#define LOG_DEBUG_SS(logger) zcs::LogEventWrap(zcs::LogEvent::ptr(new zcs::LogEvent( \
                    logger, zcs::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, 3))).GetEvent()->GetSS()

#define LOG_DEBUG(format, ...) zcs::Logger::GetInstance()->Log(zcs::LogLevel::Level::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  zcs::Logger::GetInstance()->Log(zcs::LogLevel::Level::INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  zcs::Logger::GetInstance()->Log(zcs::LogLevel::Level::WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) zcs::Logger::GetInstance()->Log(zcs::LogLevel::Level::ERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) zcs::Logger::GetInstance()->Log(zcs::LogLevel::Level::FATAL, format, ##__VA_ARGS__)

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
            ,uint32_t fiber_id, uint64_t time);
    
    const char* GetFile() const { return file_;}
    int32_t GetLine() const { return line_;}
    uint32_t GetElapse() const { return elapse_;}
    uint32_t GetThreadId() const { return thread_id_;}
    uint32_t GetFiberId() const { return fiber_id_;}
    uint64_t GetTime() const { return time_;}
    std::shared_ptr<Logger> GetLogger() const { return logger_;}
    LogLevel::Level GetLevel() const { return level_;}
    std::stringstream& GetSS() { return ss_;}


    void Format(const char* format, ...);
    void Format(const char* format, va_list vl);
private:
    const char* file_;
    int32_t line_;
    uint32_t elapse_;
    uint32_t thread_id_;
    uint32_t fiber_id_;
    uint64_t time_;
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_;
    std::stringstream ss_;
};

class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;
    static Logger* GetInstance() {
        static Logger instance;
        return &instance;
    }

    Logger();
    ~Logger();
    void Log(LogLevel::Level level, LogEvent::ptr event);
    uint32_t Init();
private:
    std::string filename_;
};



class LogFormat {

};

class LogAppender {
public:
private:
};

class StdoutLogAppender : public LogAppender {

};

class FileLogAppender : public LogAppender {

};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr event);
    ~LogEventWrap();
    std::shared_ptr<LogEvent> GetEvent() const { return event_;}
private:
    LogEvent::ptr event_;
};

}



#endif //__LOG_H__