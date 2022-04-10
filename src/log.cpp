#include "log.h"
#include <iostream>
#include <stdarg.h>
namespace zcs {

/************************************* Logger **********************************************/
Logger::Logger() {

}

Logger::~Logger() {

}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {

    // va_list valist;
    // va_start(valist, format);

    // char test_buf[512];
    // int m = snprintf(test_buf, 10, "[%s] ", LogLevel::ToString(level));
    // int n = vsnprintf(test_buf + m, 511, format, valist);
    // //test_buf[m + n] = '\n';
    // test_buf[m + n] = '\0';
    // printf("%s", test_buf);
    std::cout << "\033[31m" <<LogLevel::ToString(level) << "\033[0m\t"
         << event->GetFile() << "\t"
         << event->GetLine() << "\t"
         << event->GetThreadId() << "\t"
         << event->GetFiberId() << "\t"
         << event->GetTime() << "\t"
         << event->GetElapse() << "\t"
         << event->GetSS().str() << "\n";
}

/************************************* LogLevel **********************************************/
const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKONW";
    }

    return "UNKONW";
}

LogLevel::Level LogLevel::FromString(const std::string& level) {
    return LogLevel::DEBUG;
}

/************************************* LogEventWrap **********************************************/
LogEventWrap::LogEventWrap(LogEvent::ptr event)
    :event_(event){
}

LogEventWrap::~LogEventWrap() {
    event_->GetLogger()->Log(event_->GetLevel(), event_);
}

/************************************* LogEvent **********************************************/
void LogEvent::Format(const char* format, ...) {
    va_list vl;
    va_start(vl, format);
    Format(format, vl);
    va_end(vl);
}

void LogEvent::Format(const char* format, va_list vl) {
    char* buf = nullptr;
    int32_t len = vasprintf(&buf, format, vl);
    printf("write to ss content %s\n", buf);
    if(len != -1) {
        ss_ << std::string(buf, len);
        free(buf);
    }
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file
            ,int32_t line, uint32_t elapse, uint32_t thread_id
            ,uint32_t fiber_id, uint64_t time)
    :file_(file)
    ,line_(line)
    ,elapse_(elapse)
    ,thread_id_(thread_id)
    ,fiber_id_(fiber_id)
    ,time_(time)
    ,level_(level)
    ,logger_(logger) {

}

}