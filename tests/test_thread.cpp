#include "log.h"
#include "thread.h"
//#include <memory>

zcs::Logger::ptr g_logger = ZCS_LOG_NAME("thread");
int count = 0;

void func1() {
    ZCS_INFO(g_logger) << "name : " << zcs::Thread::GetCurName()
                       << " this.name : " <<  zcs::Thread::GetThis()->GetName()
                       << " id : " << zcs::GetThreadId()
                       << " this.id " << zcs::Thread::GetThis()->GetId();

    for(int i = 0; i < 100000; i++) {
        count++;
    }
}

void func2() {
    int count = 0;
    while(count < 10) {
        ZCS_DEBUG(g_logger) << "********************************************";
        count++;
    }
}

void func3() {
    while(true) {
        ZCS_DEBUG(g_logger) << "---------------------------------------------";
    }
}

int main(int argc, char* argv[]) {
    ZCS_DEBUG(g_logger) << "test thread begin";
    std::vector<zcs::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i) {
        zcs::Thread::ptr thr(new zcs::Thread(&func1, "TEST_" + std::to_string(i * 2)));
        //zcs::Thread::ptr thr2(new zcs::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        //thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->Join();
    }
    ZCS_DEBUG(g_logger) << "test thread end";
    return 0;
}