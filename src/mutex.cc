#include "mutex.h"
#include <iostream>

namespace zcs {

Semaphore::Semaphore(uint32_t count) {
    if(sem_init(&semaphore_, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&semaphore_);
}

void Semaphore::Wait() {
    if(sem_wait(&semaphore_)) {
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::Notify() {
    if(sem_post(&semaphore_)) {
        throw std::logic_error("sem_post error");
    }
}

} // namespace zcs
