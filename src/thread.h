/**
 * @file thread.h
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
#ifndef __THREAD_H__
#define __THREAD_H__

#include <functional>
#include <signal.h>
#include <pthread.h>
#include <memory>

namespace zcs {
/**
 * @brief 
 * 
 * 
 * @details  
 */
class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    pid_t GetId() const { return id_;}
    const std::string& GetName() const { return name_;}

    void Join();
    static Thread* GetThis();
    static const std::string& GetCurName();
    static void SetCurName(const std::string& name);
private:
    static void* Run(void* arg);
private:
    pid_t id_ = -1;
    pthread_t thread_;
    std::function<void()> cb_;
    std::string name_;
};

} // namespace zcs


#endif //__THREAD_H__