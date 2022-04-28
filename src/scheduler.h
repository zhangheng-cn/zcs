/**
 * @file scheduler.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-28 <th>1.0 <th>zhcs    <th>create
 * </table>
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <memory>
#include <vector>
#include <list>
#include <functional>

#include "fiber.h"
#include "log.h"
#include "mutex.h"
#include "thread.h"

namespace zcs {

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool user_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& GetName() const { return name_;}

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void Start();
    void Stop();

    template<class Task>
    void Schedule(Task t, int thread = -1) {
        //TODO
    }

    template<class TaskIterator>
    void Schedule(TaskIterator begin, TaskIterator end) {
        //TODO
    }

protected:
    virtual void Tickle();
    virtual bool Stopping();
    virtual void Idle();

    void Run();
    void SetThis();

private:
    template<class Task>
    void ScheduleNoLock(Task t, int thread) {
        //TODO
    }

private:
    /**
     * @brief task is fiber or function
     * 
     * 
     * @details 
     */
    struct Task {
        Fiber::ptr fiber;
        std::function<void()> cb;
        // task希望执行在哪个线程上, -1代表无
        int thread;

        Task(Fiber::ptr f, int thr) 
            :fiber(f), thread(thr) {

        }

        Task(Fiber::ptr* f, int thr) 
            :thread(thr) {
            fiber.swap(*f);
        }

        Task(std::function<void()> f, int thr) 
            :cb(f), thread(thr) {

        }

        Task(std::function<void()>* f, int thr) 
            :thread(thr) {
            cb.swap(*f);
        }

        Task()
            :thread(-1) {

        }

        void Reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    MutexType mutex_;
    std::vector<Thread> threads_;
    std::list<Task> tasks_;
    Fiber::ptr root_fiber_;
    std::string name_;
protected:
    std::vector<int> thread_ids_;
    size_t thread_count = 0;
    std::atomic<size_t> active_thread_count = {0};
    std::atomic<size_t> idle_thread_count = {0};
    bool stopping_ = true;
    bool auto_stop = false;
    int root_thread = 0;
};


} // namespace zcs

#endif //__SCHEDULER_H__