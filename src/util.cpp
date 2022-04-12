#include <syscall.h>
#include "util.h"

namespace zcs {

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

pid_t GetFiberId() {
    // TODO
    return 888;
}

}