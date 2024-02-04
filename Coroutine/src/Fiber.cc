#include "../include/Fiber.h"
#include <unistd.h>

namespace Coroutine {

static std::atomic<uint64_t> s_fiber_count = 0;
static std::atomic<uint64_t> s_fiber_id = 0;

// 当前线程正在运行的协程
static thread_local Fiber* t_fiber = nullptr;
// 当前线程的主协程
static thread_local Fiber::ptr t_thread_fiber = nullptr;

using StackAllocator = MallocStackAllocator;

Fiber::Fiber()
{
    ret_ = 0;
    SetThis(this);
    m_state_ = RUNNING;
    ret_ = getcontext(&m_ctx_);
    ERROR_CHECK(ret_, -1, "getcontext");
    ++s_fiber_count;
    m_id_ = s_fiber_id++;

    fprintf(stderr, "Out Fiber()\n");
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler)
    : m_id_(s_fiber_id++), m_cb_(cb), m_runInScheduler_(run_in_scheduler)
{
    ret_ = 0;
    ++s_fiber_count;
    // m_stacksize_ = stacksize?stacksize : g_fiber_stack_size->getValue();
    // 大坑：栈空间分配不够，栈向下占用其他内存空间,导致其他成员变量异常，最终导致core dump！！
    m_stacksize_ = stacksize ? stacksize : 4096;
    m_stack_ = StackAllocator::Alloc(m_stacksize_);

    ret_ = getcontext(&m_ctx_);
    ERROR_CHECK(ret_, -1, "getcontext");
    m_ctx_.uc_link = nullptr;
    m_ctx_.uc_stack.ss_sp = m_stack_;
    m_ctx_.uc_stack.ss_size = m_stacksize_;
    makecontext(&m_ctx_, &Fiber::MainFunc, 0);

    fprintf(stderr, "Out Fiebr(cb)\n");
}

Fiber::~Fiber() {
    -- s_fiber_count;
    if (m_stack_) {
        assert(m_state_ == TERM);
        StackAllocator::Dealloc(m_stack_, m_stacksize_);
    } else {
        assert(!m_cb_);
        assert(m_state_ == RUNNING);

        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    // fprintf(stderr, "Fiber::~Fiber id = %ld, total = %ld\n", s_fiber_id, s_fiber_count);
    // std::cout << "Fiber::~Fiber id = "<< s_fiber_id << ", total = " << s_fiber_count << "\n";
}

// 重置协程是为了重复利用已结束运行的协程的栈空间,创建新的协程
//todo 对READY状态的协程也应该能够RESET
void Fiber::Reset(std::function<void()> cb) {
    assert(m_stack_);
    assert(m_state_ == TERM);
    m_cb_ = cb;
    ret_ = getcontext(&m_ctx_);
    ERROR_CHECK(ret_, -1, "getcontext");

    // 这里我理解是重新获取下最新的上下文，复用栈空间
    m_ctx_.uc_link = nullptr;
    m_ctx_.uc_stack.ss_sp = m_stack_;
    m_ctx_.uc_stack.ss_size = m_stacksize_;

    makecontext(&m_ctx_, &Fiber::MainFunc, 0);
    m_state_ = READY;
}

// 获取执行权限
void Fiber::Resume() {
    assert(m_state_ != TERM && m_state_ != RUNNING);

    SetThis(this);
    m_state_ = RUNNING;

    /**
     * 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    */
    if (m_runInScheduler_) {
        ret_ = swapcontext(&(Scheduler::GetMainFiber()->m_ctx_), &m_ctx_);
        ERROR_CHECK(ret_, -1, "swapcontext");
    } else {
        ret_ = swapcontext(&(t_thread_fiber->m_ctx_), &m_ctx_);
        ERROR_CHECK(ret_, -1, "swapcontext");
    }

}

// 让出执行权限
void Fiber::Yield() {
    assert(m_state_ == TERM || m_state_ == RUNNING);
    SetThis(t_thread_fiber.get());
    if (m_state_ != TERM) {
        m_state_ = READY;
    }
    /**
     * 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    */
    if (m_runInScheduler_) {
        ret_ = swapcontext(&m_ctx_, &(Scheduler::GetMainFiber()->m_ctx_));
        ERROR_CHECK(ret_, -1, "swapcontext");
    } else {
        ret_ = swapcontext(&m_ctx_, &(t_thread_fiber->m_ctx_));
        ERROR_CHECK(ret_, -1, "swapcontext");
    }
}

void Fiber::SetThis(Fiber *f) {
    t_fiber = f;
}

/**
 * 返回当前线程正在执行的协程,
 * 如果当前线程没有协程，则创建一个主协程
*/
Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber);
    assert(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

/**
 * 协程入口函数。
 * why?可以方便的在协程执行完毕后自动yeild
*/
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ERROR_CHECK(cur, nullptr, "GetThis");

    // 真正的执行协程
    try{
        cur->m_cb_();
        cur->m_cb_ = nullptr;
        cur->m_state_ = TERM;
    } catch (...){
        std::cout << "gg" << std::endl;
    }

    // 手动让t_fiber的引用计数-1
    auto raw_ptr = cur.get();
    cur.reset();
    // 自动yeild
    raw_ptr->Yield();
}

uint64_t Fiber::GetFiberId() {
    return s_fiber_id;
}

} // end of namespace Coroutine