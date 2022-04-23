/**
 * @file thread_pool.h
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
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include "thread.h"

namespace zcs {

class ThreadPool {
public:
    ThreadPool(uint32_t thread_number = 8, uint32_t max_request = 10000);
    ~ThreadPool();
private:
    void Run();
    static void Worker();
private:
    uint32_t thread_number_;
    uint32_t max_request_;
    std::vector<Thread::ptr> threads_;
};


} // namespace zcs


#endif //__THREAD_POOL_H__