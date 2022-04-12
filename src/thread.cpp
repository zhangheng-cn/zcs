#include "thread.h"
#include "log.h"

namespace zcs {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKONW";
static zcs::Logger::ptr g_logger = ZCS_LOG_NAME("system");
    
void Thread::Join() {
    if(thread_) {
        int ret = pthread_join(thread_, nullptr);
        if(ret) {
            ZCS_ERROR(g_logger) << "pthread_join thread failed ! ret=" << ret
                            << " name=" << name_;
            throw std::logic_error("pthread_join error.");
        }
        thread_ = 0;
    }
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    :cb_(cb)
    ,name_(name) {
    ZCS_INFO(g_logger) << "Thread constructor!";
    if(name.empty()) {
        name_ = "UNKONW";
    }
    int ret = pthread_create(&thread_, nullptr, &Thread::Run, this);
    if(ret) {
        ZCS_ERROR(g_logger) << "pthread_create thread failed ! ret=" << ret
                            << " name=" << name;
        throw std::logic_error("pthread_create error.");
    }
}

Thread::~Thread() {
    if(thread_) {
        pthread_detach(thread_);
    }
}

Thread* Thread::GetThis() {
    return t_thread;
} 
const std::string& Thread::GetCurName() {
    return t_thread_name;
}

void Thread::SetCurName(const std::string& name) {
    if(name.empty()) {
        return;
    }
    if(t_thread) {
        t_thread->name_ = name;
    }
    t_thread_name = name;
}

void* Thread::Run(void* arg) {
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->name_;
    t_thread->id_ = zcs::GetThreadId();
    pthread_setname_np(pthread_self(), thread->name_.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->cb_);

    cb();
    return 0;
}

} // namespace zcs
