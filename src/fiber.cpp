#include "fiber.h"

#include <atomic>

#include "zcs.h"


namespace zcs {

static Logger::ptr g_logger = ZCS_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;               // 当前协程
static thread_local Fiber::ptr t_thread_fiber = nullptr;    // 主协程

static ConfigVar<uint32_t>::ptr g_conig_fiber_stacksize = 
    Config::LookUp<uint32_t>("fiber.stacksize", 1024 * 1024, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber() {
    state_ = EXEC;
    SetThis(this);

    if(getcontext(&ctx_)) {
        ZCS_ASSERT2(false, "getcontext error");
    }
    ++s_fiber_count;
    ZCS_DEBUG(g_logger) << "Fiber::Fiber main_fiber_id=" << id_ << " total fiber: " << TotalFibers();
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    : id_(++s_fiber_id)
    ,cb_(cb) {
    ++s_fiber_count;
    stacksize_ = stacksize ? stacksize : g_conig_fiber_stacksize->GetValue();
    
    stack_ = StackAllocator::Alloc(stacksize_);
    if(getcontext(&ctx_)) {
        ZCS_ASSERT2(false, "getcontext error");
    }

    ctx_.uc_link = nullptr;
    ctx_.uc_stack.ss_sp = stack_;
    ctx_.uc_stack.ss_size = stacksize_;

    makecontext(&ctx_, &Fiber::MainFunc, 0);
    ZCS_DEBUG(g_logger) << "Fiber::Fiber fiber_id=" << id_ << " total fiber: " << TotalFibers();
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(stack_) {
        ZCS_ASSERT( state_ == INIT || state_ == TERM || state_ == EXECPT);
        StackAllocator::Dealloc(stack_, stacksize_);
    } else {
        ZCS_ASSERT(!cb_);
        ZCS_ASSERT(state_ == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    ZCS_DEBUG(g_logger) << "Fiber::~Fiber fiber_id=" << id_ << " total fiber: " << TotalFibers();
}

void Fiber::Reset(std::function<void()> cb) {
    ZCS_ASSERT(stack_);
    ZCS_ASSERT(state_ == INIT || state_ == TERM || state_ == EXECPT);
    cb_ = cb;

    if(getcontext(&ctx_)) {
        ZCS_ASSERT2(false, "getcontext error");
    }

    ctx_.uc_link = nullptr;
    ctx_.uc_stack.ss_sp = stack_;
    ctx_.uc_stack.ss_size = stacksize_;

    makecontext(&ctx_, &Fiber::MainFunc, 0);
    state_ = INIT;
}

void Fiber::SwapIn() {
    SetThis(this);
    ZCS_ASSERT(state_ != EXEC);
    state_ = EXEC;
    if(swapcontext(&t_thread_fiber->ctx_, &ctx_)) {
        ZCS_ASSERT2(false, "swapcontext error");
    }
}

void Fiber::SwapOut() {
    SetThis(t_thread_fiber.get());
    if(swapcontext(&ctx_, &t_thread_fiber->ctx_)) {
        ZCS_ASSERT2(false, "swapcontext error");
    }
}



Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber);
    ZCS_ASSERT(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    ZCS_ASSERT(cur->state_ == EXEC);
    cur->state_ = HOLD;
    cur->SwapOut();
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    ZCS_ASSERT(cur->state_ == EXEC);
    cur->state_ = READY;
    cur->SwapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->GetId();
    }
    return 0;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ZCS_ASSERT(cur);
    
    try {
        cur->cb_();
        cur->cb_ == nullptr;
        cur->state_ = TERM;
    } catch (std::exception& ex) {
        ZCS_ERROR(g_logger) << "Fiber exception: " << ex.what()
                            << " fiber id= " << cur->id_
                            << std::endl
                            << zcs::BacktraceToString();
    } catch (...) {
        cur->state_ = EXECPT;
        ZCS_ERROR(g_logger) << "Fiber exception: "
                    << " fiber id= " << cur->id_
                    << std::endl
                    << zcs::BacktraceToString();
    }

    // 获取裸指针，防止share ptr引用计数大于0 fiber不能正常析构
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->SwapOut();

    ZCS_ASSERT2(false, "never reach!");
}

} // namespace zcs
