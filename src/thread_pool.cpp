#include <unistd.h>
#include <iostream>

#include "thread_pool.h"
#include "log.h"

static zcs::Logger::ptr log_server = ZCS_LOG_NAME("thread_pool");
static uint64_t test_count = 0;

namespace zcs {
ThreadPool::ThreadPool(uint32_t thread_number, uint32_t max_request)
    :thread_number_(thread_number)
    ,max_request_(max_request) {
    if(thread_number_ <= 0 || max_request_ <= 0) {
        throw std::logic_error("thread_pool config error.");
    }
    for(int i = 0; i < thread_number_; i++) {
        threads_.push_back(Thread::ptr(new Thread(&Worker, "SERVER_" + std::to_string(i))));
    }
    for(auto& iter : threads_) {
        iter->Join();
    }
}

ThreadPool::~ThreadPool() {

}

void ThreadPool::Run() {
    
}

void ThreadPool::Worker() {
    while(1) {
        test_count++;
        std::cout << test_count;
        ZCS_DEBUG(log_server) << "thread pool worker!!!!! count=" << test_count;
        //sleep(5);
    }
}

} // namespace zcs
