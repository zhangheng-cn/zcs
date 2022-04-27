/**
 * @file fiber.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-27
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-27 <th>1.0 <th>zhcs    <th>create
 * </table>
 */
#ifndef __FIBER_H__
#define __FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

namespace zcs {

/**
 * @brief 
 * 
 * 
 * @details 1.std::enable_shared_from_this此处用途：防止在栈上创建对象？ 
 */
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXECPT
    };

    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();
    
    void Reset(std::function<void()> cb);
    void SwapIn();
    void SwapOut();
    
    uint64_t GetId() const { return id_;}
    State GetState() const { return state_;}
private:
    /**
     * @brief Construct a new Fiber object
     * 
     * 
     * @details 主协程的初始化
     */
    Fiber();
public:
    static Fiber::ptr GetThis();
    static void SetThis(Fiber* f);
    static void YieldToHold();
    static void YieldToReady();
    static uint64_t TotalFibers();
    static void MainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t id_ = 0;           // 协程id
    uint32_t stacksize_ = 0;    // 协程栈大小
    State state_ = INIT;        // 协程状态
    ucontext_t ctx_;            // 上下文
    void* stack_ = nullptr;     // 协程栈

    std::function<void()> cb_;  // 协程函数
};

} // namespace zcs

#endif //__FIBER_H__