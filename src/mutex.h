#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include "nocopyable.h"

namespace zcs {
    
template <class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex)
        :mutex_(mutex) {
        mutex_.Lock();
        locked_ = true;
    }

    ~ScopedLockImpl() {
        Unlock();
    }

    void Lock() {
        if(!locked_) {
            mutex_.Lock();
            locked_ = true;
        }
    }

    void Unlock() {
        if(locked_) {
            mutex_.Unlock();
            locked_ = false;
        }
    }
private:
    T& mutex_;
    bool locked_;
};

class Mutex : public Nocopyable {
public:
    typedef ScopedLockImpl<Mutex> LOCK;
    Mutex() {
        pthread_mutex_init(&mutex_, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }
    void Lock() {
        pthread_mutex_lock(&mutex_);
    }
    void Unlock() {
        pthread_mutex_unlock(&mutex_);
    }
private:
    pthread_mutex_t mutex_;
};

} // namespace zcs


#endif //__MUTEX_H__