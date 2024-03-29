// linux
#include <ucontext.h>
#include <assert.h>

// cpp
#include <memory>
#include <functional>
#include <atomic>

// my
#include "debug.h"

#ifndef __FIBER__
#define __FIBER__

namespace Coroutine {

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State
    {
        READY,
        RUNNING,
        TERM
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = true);
    ~Fiber();
    void Reset(std::function<void()> cb);
    void Resume();
    void Yield();
    uint64_t GetId() const
    {
        return m_id_;
    }
    State GetState() const
    {
        return m_state_;
    }

public:
    static void SetThis(Fiber *f);
    static Fiber::ptr GetThis();
    static uint64_t TotalFibers();
    static void MainFunc();
    static uint64_t GetFiberId();

private:
    uint64_t m_id_ = 0;
    std::function<void()> m_cb_;
    bool m_runInScheduler_;
    uint32_t m_stacksize_ = 0;
    State m_state_ = READY;
    ucontext_t m_ctx_;
    void *m_stack_ = nullptr;
    int ret_;
};

} // end of namespace Coroutine

#endif