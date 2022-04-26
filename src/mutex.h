/**
 * @file mutex.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-21
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-21 <th>1.0 <th>zhcs    <th>create
 * </table>
 */
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include "nocopyable.h"

namespace zcs {
    

class Semaphore : Nocopyable {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void Wait();
    void Notify();
private:
    sem_t semaphore_;
};

template <class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex)
        :mutex_(mutex) {
        mutex_.lock();
        locked_ = true;
    }

    ~ScopedLockImpl() {
        Unlock();
    }

    void Lock() {
        if(!locked_) {
            mutex_.lock();
            locked_ = true;
        }
    }

    void Unlock() {
        if(locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }
private:
    T& mutex_;
    bool locked_;
};

template <class T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex)
        :mutex_(mutex) {
        mutex_.rdlock();
        locked_ = true;
    }

    ~ReadScopedLockImpl() {
        Unlock();
    }

    void Lock() {
        if(!locked_) {
            mutex_.rdlock();
            locked_ = true;
        }
    }

    void Unlock() {
        if(locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }
private:
    T& mutex_;
    bool locked_;
};

template <class T>
class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex)
        :mutex_(mutex) {
        mutex_.wrlock();
        locked_ = true;
    }

    ~WriteScopedLockImpl() {
        Unlock();
    }

    void Lock() {
        if(!locked_) {
            mutex_.wrlock();
            locked_ = true;
        }
    }

    void Unlock() {
        if(locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }
private:
    T& mutex_;
    bool locked_;
};

class Mutex : public Nocopyable {
public:
    typedef ScopedLockImpl<Mutex> Lock;
    Mutex() {
        pthread_mutex_init(&mutex_, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }
    void lock() {
        pthread_mutex_lock(&mutex_);
    }
    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }
private:
    pthread_mutex_t mutex_;
};

class NullMutex : Nocopyable {
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}
};

class RWMutex : public Nocopyable {
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex() {
        pthread_rwlock_init(&lock_, nullptr);
    }
    ~RWMutex() {
        pthread_rwlock_destroy(&lock_);
    }
    void rdlock() {
        pthread_rwlock_rdlock(&lock_);
    }
    void wrlock() {
        pthread_rwlock_wrlock(&lock_);
    }
    void unlock() {
        pthread_rwlock_unlock(&lock_);
    }
private:
    pthread_rwlock_t lock_;
};

class NullRWMutex : Nocopyable {
public:
    NullRWMutex() {}
    ~NullRWMutex() {}
    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};


} // namespace zcs


#endif //__MUTEX_H__