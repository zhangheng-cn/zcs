#include "log.h"
#include <iostream>
#include <stdarg.h>
#include <time.h>
#include <functional>
#include "config.h"

namespace zcs {

/************************************* Logger **********************************************/
Logger::Logger(const std::string& name)
    :name_(name)
    ,level_(LogLevel::DEBUG) {
    formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
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
    // std::cout << "\033[31m" <<LogLevel::ToString(level) << "\t"
    //      << event->GetFile() << "\t"
    //      << event->GetLine() << "\t"
    //      << event->GetThreadId() << "\t"
    //      << event->GetFiberId() << "\t"
    //      << event->GetTime() << "\t"
    //      << event->GetElapse() << "\t"
    //      << event->GetSS().str() << "\n";
    auto self = shared_from_this();
    MutexType::LOCK lock(mutex_);
    if(!appenders_.empty()) {
        for(auto& iter : appenders_) {
            iter->Log(self, level, event);
        }
    } else {
        root_->Log(level, event);
    }
}

void Logger::AddAppender(LogAppender::ptr appender) {
    MutexType::LOCK lock(mutex_);
    if(!appender->GetFormatter()) {
        appender->formatter_ = formatter_;
    }
    appenders_.push_back(appender);
}

void Logger::DelAppender(LogAppender::ptr appender) {
    MutexType::LOCK lock(mutex_);
    for(auto it = appenders_.begin(); it != appenders_.end(); it++) {
        if(*it == appender) {
            appenders_.erase(it);
            break;
        }
    }
}

void Logger::ClearAppender() {
    MutexType::LOCK lock(mutex_);
    appenders_.clear();
}

void Logger::SetFormatter(const std::string& format) {
    LogFormatter::ptr new_format(new LogFormatter(format));
    if(new_format->IsError()) {
        std::cout << "Logger SetFormatter name=" << name_
                  << " value=" << format << " invalid formatter\n";
        return;
    }
    SetFormatter(new_format);
}

void Logger::SetFormatter(LogFormatter::ptr format) {
    MutexType::LOCK lock(mutex_);
    formatter_ = format;
    for(auto& it : appenders_) {
        if(!it->has_formatter_) {
            it->formatter_ = formatter_;
        }
    }
}

LogFormatter::ptr Logger::GetFormatter() {
    MutexType::LOCK lock(mutex_);
    return formatter_;
}

/************************************* LogLevel **********************************************/
const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
    case LogLevel::DEBUG:
        return "DEBUG";
        break;
    case LogLevel::INFO:
        return "INFO";
        break;
    case LogLevel::WARN:
        return "WARN";
        break;
    case LogLevel::ERROR:
        return "ERROR";
        break;
    case LogLevel::FATAL:
        return "FATAL";
        break;
    default:
        return "UNKONW";
    }

    return "UNKONW";
}



LogLevel::Level LogLevel::FromString(const std::string& level) {
    if(level == "debug" || level == "DEBUG") {
        return LogLevel::DEBUG;
    }
    if(level == "info" || level == "INFO") {
        return LogLevel::INFO;
    }
    if(level == "warn" || level == "WARN") {
        return LogLevel::WARN;
    }
    if(level == "error" || level == "ERROR") {
        return LogLevel::ERROR;
    }
    if(level == "fatal" || level == "FATAL") {
        return LogLevel::FATAL;
    }
    return LogLevel::UNKONW;
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
    if(len != -1) {
        ss_ << std::string(buf, len);
        free(buf);
    }
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file
            ,int32_t line, uint32_t elapse, uint32_t thread_id
            ,uint32_t fiber_id, uint64_t time, const std::string& thread_name)
    :file_(file)
    ,line_(line)
    ,elapse_(elapse)
    ,thread_id_(thread_id)
    ,fiber_id_(fiber_id)
    ,time_(time)
    ,thread_name_(thread_name)
    ,logger_(logger)
    ,level_(level) {

}
/************************************* LogAppender **********************************************/
void LogAppender::SetFormatter(LogFormatter::ptr formatter) {
    MutexType::LOCK lock(mutex_);
    formatter_ = formatter;
    if(formatter_) {
        has_formatter_ = true;
    } else {
        has_formatter_ = false;
    }
}

LogFormatter::ptr LogAppender::GetFormatter() {
    MutexType::LOCK lock(mutex_);
    return formatter_;
}

/************************************* StdoutLogAppender **********************************************/
void StdoutLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    MutexType::LOCK lock(mutex_);
    formatter_->Format(std::cout, logger, level, event);
}
/************************************* FormatItem **********************************************/

class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(event->GetLevel());
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetLogger()->GetName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetThreadId();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\n";
    }
};

class TimeFormatItem : public LogFormatter::FormatItem {
public:
    TimeFormatItem(const std::string& time_fmt = "%Y-%m-%d %H:%M:%S") 
        :format_(time_fmt) {
        if(format_.empty()) {
            format_ = "%Y-%m-%d %H:%M:%S";
        }
    }
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->GetTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), format_.c_str(), &tm);
        os << buf;
    }
private:
    std::string format_;
};

class FileNameFormatItem : public LogFormatter::FormatItem {
public:
    FileNameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetLine();
    }
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
   FiberIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetThreadName();
    }
};

// for error 
class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};
/************************************* LogFormatter **********************************************/
std::string LogFormatter::Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto& iter : items_) {
        iter->Format(ss, logger, level, event);
    }
    return ss.str();
}

std::ostream& LogFormatter::Format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    for(auto& iter : items_) {
        iter->Format(ofs, logger, level, event);
    }
    return ofs;
}

LogFormatter::LogFormatter(const std::string& pattern)
    :pattern_(pattern) {
    Init();
}

/**
 * @brief 
 * 
 * 
 * @details default : %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
 *          1.%xx 2.%xx{xx} 3.%%
 */
void LogFormatter::Init() {
    static std::map<std::string ,std::function<FormatItem::ptr(const std::string&)>> itemMap{
            {"m", [] (const std::string& str) { return MessageFormatItem::ptr(new MessageFormatItem(str));}},
            {"p", [] (const std::string& str) { return LevelFormatItem::ptr(new LevelFormatItem(str));}},
            {"r", [] (const std::string& str) { return ElapseFormatItem::ptr(new ElapseFormatItem(str));}},
            {"c", [] (const std::string& str) { return NameFormatItem::ptr(new NameFormatItem(str));}},
            {"t", [] (const std::string& str) { return ThreadIdFormatItem::ptr(new ThreadIdFormatItem(str));}},
            {"n", [] (const std::string& str) { return NewLineFormatItem::ptr(new NewLineFormatItem(str));}},
            {"d", [] (const std::string& str) { return TimeFormatItem::ptr(new TimeFormatItem(str));}},
            {"f", [] (const std::string& str) { return FileNameFormatItem::ptr(new FileNameFormatItem(str));}},
            {"l", [] (const std::string& str) { return LineFormatItem::ptr(new LineFormatItem(str));}},
            {"T", [] (const std::string& str) { return TabFormatItem::ptr(new TabFormatItem(str));}},
            {"F", [] (const std::string& str) { return FiberIdFormatItem::ptr(new FiberIdFormatItem(str));}},
            {"N", [] (const std::string& str) { return ThreadNameFormatItem::ptr(new ThreadNameFormatItem(str));}},
    };

    std::string str;
    for(size_t i = 0; i < pattern_.size(); i++){
        if(pattern_[i] != '%'){
            str.push_back(pattern_[i]);
            continue;
        }
        if(i + 1 >= pattern_.size()){
            items_.push_back(StringFormatItem::ptr(new StringFormatItem("<error format %>")));
            error_ = true;
            continue;
        }
        i++;
        if(pattern_[i] == '%'){
            str.push_back('%');
            continue;
        }
        if(str.size()){
            items_.push_back(StringFormatItem::ptr(new StringFormatItem(str)));
            str.clear();
        }
        if(pattern_[i] != 'd'){
            std::string tmp(1,pattern_[i]);
            auto it = itemMap.find(tmp);
            if(it == itemMap.end()){
                items_.push_back(StringFormatItem::ptr(new StringFormatItem("<error format %"+tmp+">")));
                error_ = true;
            }else{
                items_.push_back(it->second(tmp));
            }
        }else{
            //解析日期
            if(i + 1 >= pattern_.size())
                continue;
            if(pattern_[i+1] != '{'){
                items_.push_back(itemMap["d"](""));
                continue;
            }
            i++;
            if(i + 1 >= pattern_.size()){
                items_.push_back(StringFormatItem::ptr(new StringFormatItem("<error format %d{ >")));
                error_ = true;
                continue;
            }
            i++;
            auto index = pattern_.find_first_of('}',i);
            if(index == std::string::npos){
                items_.push_back(StringFormatItem::ptr(new StringFormatItem("<error format %d{" + pattern_.substr(i) + " >")));
                error_ = true;
                continue;
            }
            items_.push_back(itemMap["d"](pattern_.substr(i,index - i)));
            i = index;
        }
    }
}


/************************************* LoggerManager **********************************************/
LoggerManager::LoggerManager() {
    root_.reset(new Logger);
    root_->AddAppender(LogAppender::ptr(new StdoutLogAppender));

    loggers_[root_->name_] = root_;

    Init();
}

Logger::ptr LoggerManager::GetLogger(const std::string& name) {
    MutexType::LOCK lock(mutex_);
    auto iter = loggers_.find(name);
    if(iter != loggers_.end()) {
        return iter->second;
    }
    
    // not find, create a new one
    Logger::ptr new_logger(new Logger(name));
    new_logger->root_ = root_;
    loggers_[name] = new_logger;
    return new_logger;
}

void LoggerManager::Init() {

}
/************************************* FileLogAppender **********************************************/
FileLogAppender::FileLogAppender(const std::string& filename)
    :filename_(filename) {
    Reopen();
}
void FileLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    uint64_t now = event->GetTime();
    if(now >= last_time_ + 3) {
        Reopen();
        last_time_ = now;
    }
    MutexType::LOCK lock(mutex_);
    if(!formatter_->Format(filestream_, logger, level, event)) {
        std::cout << "error\n";
    }
}
bool FileLogAppender::Reopen() {
    MutexType::LOCK lock(mutex_);
    if(filestream_) {
        filestream_.close();
    }
    filestream_.open(filename_.c_str(), std::ios::app);
    // if(!filestream_.is_open()) {
    //     std::string dir = 
    // }
    //     ofs.open(filename.c_str(), mode);   
    // if(!ofs.is_open()) {
    //     std::string dir = Dirname(filename);
    //     Mkdir(dir);
    //     ofs.open(filename.c_str(), mode);
    // }
    return filestream_.is_open();
}

/************************************* Log config **********************************************/

struct LogAppenderDefine {
    int32_t type = 0; // file 1, stdout 2
    LogLevel::Level level = LogLevel::UNKONW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKONW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};

template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        LogDefine ld;
        if(!node["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << node << "\n";
            throw std::logic_error("log config name is null");
        }
        ld.name = node["name"].as<std::string>();
        ld.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
        if(node["formatter"].IsDefined()) {
            ld.formatter = node["formatter"].as<std::string>();
        }

        if(node["appenders"].IsDefined()) {
            for(size_t i = 0; i < node["appenders"].size(); i++) {
                auto a = node["appenders"][i];
                if(!a["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null, " << a << "\n";
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                LogAppenderDefine lad;
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {
                        std::cout << "log config error: appender file is null, " << a << "\n";
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                    if(a["formatter"].IsDefined()) {
                        lad.file = a["formatter"].as<std::string>();
                    }
                } else if(type == "StdoutLogAppender") {
                    lad.type = 2;
                    if(a["formatter"].IsDefined()) {
                        lad.file = a["formatter"].as<std::string>();
                    }
                } else {
                    std::cout << "log config error: appender type is invalid, " << a << "\n";
                    continue;
                }
                ld.appenders.push_back(lad);
            }
        }
        return ld;
    }
};

template<>
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& v) {
        YAML::Node node;
        node["name"] = v.name;
        if(v.level != LogLevel::UNKONW) {
            node["level"] = LogLevel::ToString(v.level);
        }
        if(!v.formatter.empty()) {
            node["formatter"] = v.formatter;
        }

        for(auto& i : v.appenders) {
            YAML::Node node_ap;
            if(i.type == 1) {
                node_ap["type"] = "FileLogAppender";
                node_ap["file"] = i.file;
            } else if(i.type == 2) {
                node_ap["type"] = "StdoutLogAppender";
            }
            if(v.level != LogLevel::UNKONW) {
                node["level"] = LogLevel::ToString(v.level);
            }
            if(!v.formatter.empty()) {
                node["formatter"] = v.formatter;
            }
            node["appenders"].push_back(node_ap);
        }

        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

zcs::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = zcs::Config::LookUp("logs", std::set<LogDefine>(), "log config");

struct LogIniter {
    LogIniter() {
        g_log_defines->AddListener(0xFFEEFF, [](const std::set<LogDefine>& new_val, const std::set<LogDefine>& old_val){
            ZCS_INFO(ZCS_LOG_ROOT()) << "***** log config changed *****";
            // (add or change)
            for(auto& i : new_val) {
                auto it = old_val.find(i);
                zcs::Logger::ptr logger;
                if(it == old_val.end()) {
                    // 新增logger
                    logger = ZCS_LOG_NAME(i.name);
                } else {
                    // 修改logger
                    if(!(i == *it)) {
                        logger = ZCS_LOG_NAME(i.name);
                    } else {
                        continue;
                    }
                }

                logger->SetLevel(i.level);
                if(!i.formatter.empty()) {
                    logger->SetFormatter(i.formatter);
                }

                logger->ClearAppender();
                for(auto& a : i.appenders) {
                    zcs::LogAppender::ptr ap;
                    // type
                    if(a.type == 1) {
                        ap.reset(new FileLogAppender(a.file));
                    } else if(a.type == 2) {
                        ap.reset(new StdoutLogAppender());
                    } else {
                        continue;
                    }
                    // level
                    ap->SetLevel(a.level);
                    // formatter
                    if(!a.formatter.empty()) {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->IsError()) {
                            ap->SetFormatter(fmt);
                        } else {
                            std::cout << "log.name=" << i.name << " appender type=" << a.type
                                      << " formatter=" << a.formatter << " is invalid" << std::endl;
                        }
                    }

                    logger->AddAppender(ap);
                }


            }
            // (del)
            for(auto& j : old_val) {
                auto it = new_val.find(j);
                if(it == new_val.end()) {
                    auto logger = ZCS_LOG_NAME(j.name);
                    logger->SetLevel((LogLevel::Level) 0);
                    logger->ClearAppender();
                }
            }
        });
    }
};

/**
 * @brief 在main函数之前初始化
 * 
 * 
 * @details 
 */
static LogIniter __log_init;


}