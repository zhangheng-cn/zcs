#include "zcs.h"
#include <ucontext.h>
#include <stdint.h>

static zcs::Logger::ptr g_logger = ZCS_LOG_ROOT();

void fiber_func() {
    ZCS_DEBUG(g_logger) << "fiber run begin, state:" << zcs::Fiber::GetThis()->GetState();
    zcs::Fiber::GetThis()->YieldToHold();
    ZCS_DEBUG(g_logger) << "fiber run end, state: " << zcs::Fiber::GetThis()->GetState();
}

void test_fiber() {
    zcs::Fiber::GetThis();
    ZCS_DEBUG(g_logger) << "main fiber run begin ";
    zcs::Fiber::ptr f(new zcs::Fiber(fiber_func));
    f->SwapIn();
    ZCS_DEBUG(g_logger) << "main fiber run ing sub fiber state : " << f->GetState();
    f->SwapIn();
    ZCS_DEBUG(g_logger) << "main fiber run end sub fiber state : " << f->GetState();
}

int main(int argc, char* argv) {
    zcs::Thread::SetCurName("main");
    ZCS_DEBUG(g_logger) << "main start";
    std::vector<zcs::Thread::ptr> thrs;
    for(int i = 0; i < 3; i++) {
        thrs.push_back(zcs::Thread::ptr(new zcs::Thread(&test_fiber, "THREAD_" + std::to_string(i))));
    }
    sleep(5);

    for(auto i : thrs) {
        i->Join();
    }
    ZCS_DEBUG(g_logger) << "main end";
    // ucontext_t ct;

    // getcontext(&ct);
    // ZCS_DEBUG(g_logger) << "test ucontext!";
    // sleep(1);
    // setcontext(&ct);

    return 0;
}