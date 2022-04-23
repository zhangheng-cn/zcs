#include "log.h"
#include <iostream>
#include "thread.h"

int main(int argc, char* argv[]) {
    std::cout << "test log main \n";
    
    zcs::Logger::ptr logger(new zcs::Logger);
    logger->AddAppender(zcs::LogAppender::ptr(new zcs::StdoutLogAppender));
    logger->AddAppender(zcs::LogAppender::ptr(new zcs::FileLogAppender("log.log")));
    
    ZCS_DEBUG_FMT(logger, "%s", "my test log!");
    ZCS_DEBUG(logger) << "my test ss log";
    ZCS_INFO(logger) << "test info";
    ZCS_WARN(logger) << "test warn";
    sleep(5);
    ZCS_ERROR(logger) << "test error";
    ZCS_FATAL(logger) << "test fatal";
    ZCS_ERROR_FMT(logger, "xxx");

    auto l = ZCS_LOG_NAME("zhcs");
    sleep(3);
    ZCS_INFO(l) << "test logger manager.";

    ZCS_INFO(l) << zcs::Thread::GetCurName();
    return 0;
}