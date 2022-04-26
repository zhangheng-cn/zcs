#include "log.h"
#include "thread.h"
//#include <memory>
#include <pthread.h>
#include <iostream>
#include "zcs.h"

zcs::Logger::ptr g_logger = ZCS_LOG_ROOT();
int count = 1;

zcs::RWMutex s_mutex;

void func1() {
    ZCS_INFO(g_logger) << "name : " << zcs::Thread::GetCurName()
                       << " this.name : " <<  zcs::Thread::GetThis()->GetName()
                       << " id : " << zcs::GetThreadId()
                       << " this.id " << zcs::Thread::GetThis()->GetId();

    for(int i = 0; i < 1000000; i++) {
        zcs::RWMutex::ReadLock lock(s_mutex);
        ++count;
    }
}

void func2() {
    while(true) {
        ZCS_DEBUG(g_logger) << "********************************************";
    }
}

void func3() {
    while(true) {
        ZCS_DEBUG(g_logger) << "---------------------------------------------";
    }
}

void *FUNC(void* arg) {
    for(int i = 0; i < 100000000; i++) {
        ++count;
        //std::cout << count << "\n";
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    ZCS_DEBUG(g_logger) << "test thread begin";
    std::vector<zcs::Thread::ptr> thrs;
    //pthread_t threads[10];
    // for(int i = 0; i < 5; ++i) {
    //     zcs::Thread::ptr thr(new zcs::Thread(&func1, "TEST_" + std::to_string(i * 2)));
    //     //zcs::Thread::ptr thr2(new zcs::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
    //     thrs.push_back(thr);
    //     //thrs.push_back(thr2);
    //     //pthread_create(&threads[i], nullptr, &FUNC, nullptr);
    // }

    // test mutex log
    YAML::Node root = YAML::LoadFile("/home/zh/code/zcs/tests/mutex.yml");
    zcs::Config::LoadFromYaml(root);
    zcs::Thread::ptr thr1(new zcs::Thread(&func2, "MUTEX_1"));
    zcs::Thread::ptr thr2(new zcs::Thread(&func3, "MUTEX_2"));
    thr1->Join();
    thr2->Join();

    // for(int i = 0; i < 10; i++) {
    //     pthread_join(threads[i], 0);
    // }

    //  for(size_t i = 0; i < thrs.size(); ++i) {
    //      thrs[i]->Join();
    //  }
    // ZCS_DEBUG(g_logger) << "count =" << count;
    return 0;
}