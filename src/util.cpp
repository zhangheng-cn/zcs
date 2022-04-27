#include "util.h"
#include <syscall.h>
#include <execinfo.h>

#include "log.h"
#include "fiber.h"

static zcs::Logger::ptr g_logger = ZCS_LOG_NAME("system"); 

namespace zcs {

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

pid_t GetFiberId() {
    return Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, uint32_t size, uint32_t skip) {
    void** array = (void**)malloc(sizeof(void*) * size);
    int32_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, size);
    if(strings == NULL) {
        ZCS_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(size_t i = skip; i < s; i++) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string BacktraceToString(uint32_t size, uint32_t skip, const std::string& perfix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); i++) {
        ss << perfix << bt[i] << std::endl;
    }

    return ss.str();
}

}