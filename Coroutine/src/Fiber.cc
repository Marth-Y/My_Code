#include "Fiber.h"

Fiber::Fiber()
{
    // SetThis(this);
    m_state_ = RUNNING;
    int ret = getcontext(&m_ctx_);
    ERROR_CHECK(ret, -1, "getcontext");
    ++s_fiber_count;
    m_id_ = s_fiber_id++;

    fprintf(stderr, "Out Fiber()\n");
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler)
:m_id_(s_fiber_id++)
,m_cb_(cb)
,m_runInScheduler_(run_in_scheduler)
{
    ++s_fiber_count;
    // m_stacksize_ = stacksize?stacksize : g_fiber_stack_size->getValue();
    m_stacksize_ = stacksize ? stacksize : 1000;
    m_stack_ = malloc(m_stacksize_);

    int ret = getcontext(&m_ctx_);
    ERROR_CHECK(ret, -1, "getcontext");
    m_ctx_.uc_link = nullptr;
    m_ctx_.uc_stack.ss_sp = m_stack_;
    m_ctx_.uc_stack.ss_size = m_stacksize_;
    makecontext(&m_ctx_, &Fiber::MainFunc, 0);

    fprintf(stderr, "Out Fiebr(cb)\n");
}

void Fiber::Reset(std::function<void()> cb)
{
}
void Fiber::Resume()
{
}
void Fiber::Yield()
{
}