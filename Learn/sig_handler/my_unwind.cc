#include <algorithm>
#include <limits>
#include <signal.h>
#include <stddef.h>
#include <unwind.h>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

template <class F> class Unwinder
{
  public:
    size_t operator()(F &f, size_t depth)
    {
        _f = &f;
        _index = -1;
        _depth = depth;
        _Unwind_Backtrace(&this->backtrace_trampoline, this);
        return static_cast<size_t>(_index);
    }

  private:
    F *_f;
    ssize_t _index;
    size_t _depth;

    static _Unwind_Reason_Code backtrace_trampoline(_Unwind_Context *ctx,
                                                    void *self)
    {
        return static_cast<Unwinder *>(self)->backtrace(ctx);
    }

    _Unwind_Reason_Code backtrace(_Unwind_Context *ctx)
    {
        if (_index >= 0 && static_cast<size_t>(_index) >= _depth)
        {
            return _URC_END_OF_STACK;
        }
        int ip_before_instruction = 0;
        uintptr_t ip = _Unwind_GetIPInfo(ctx, &ip_before_instruction);
        if (!ip_before_instruction)
        {
            if (ip == 0)
            {
                ip = std::numeric_limits<uintptr_t>::max();
            }
            else
            {
                ip -= 1;
            }
        }

        if (_index >= 0)
        {
            (*_f)(static_cast<size_t>(_index), reinterpret_cast<void *>(ip));
        }
        _index += 1;
        return _URC_NO_REASON;
    }
};

template <class F> size_t unwind(F f, size_t depth)
{
    Unwinder<F> unwinder;
    return unwinder(f, depth);
}

class StackTraceImp
{
  public:
    size_t load_here(size_t depth = 32, void *context = nullptr,
                     void *error_addr = nullptr)
    {
        _stacktrace.resize(depth); // 层级数
        size_t trace_cnt = unwind(callback(*this), depth);
        _stacktrace.resize(depth);
        int index = 0;
        for (auto p : _stacktrace) {
            printf("%i:%p\n", index ++, p);
        }
        return depth;
    }
    size_t load_from(void *addr, size_t depth = 32, void *context = nullptr,
                     void *error_addr = nullptr)
    {
        return load_here(depth + 8, context, error_addr);
    }

  private:
    std::vector<void *> _stacktrace;
    struct callback
    {
        StackTraceImp &self;
        callback(StackTraceImp &_self) : self(_self) {}

        void operator()(size_t idx, void *addr)
        {
            self._stacktrace[idx] = addr;
        }
    };
};

class StackTrace : public StackTraceImp
{
};

void test_func(int sig, siginfo_t* info, void* ctx) {
    ucontext_t *uctx = static_cast<ucontext_t *>(ctx);
    StackTrace test;
    void *error_addr = nullptr;
    error_addr = reinterpret_cast<void *>(uctx->uc_mcontext.gregs[REG_RIP]); 
    test.load_from(error_addr, 32, reinterpret_cast<void*>(uctx), info->si_addr);
}

int main() { 
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_flags = static_cast<int>(SA_SIGINFO | SA_ONSTACK | SA_NODEFER | SA_RESETHAND);
    sigfillset(&action.sa_mask);
    sigdelset(&action.sa_mask, SIGINT);
    action.sa_sigaction = &test_func;
    sigaction(SIGINT, &action, nullptr);

    while (true)
        {};
}