#ifndef __SIG_HANDLING__
#define __SIG_HANDLING__

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <vector>

#define NOINLINE __attribute__((noinline))
#if defined(__linux) || defined(__linux__)
#define BACKWARD_SYSTEM_LINUX
#endif

template <typename T> struct rm_ptr
{
    typedef T type;
};

template <typename T> struct rm_ptr<T *>
{
    typedef T type;
};

template <typename T> struct rm_ptr<const T *>
{
    typedef const T type;
};

class StackTraceImplBase
{
  public:
    StackTraceImplBase()
        : _thread_id(0), _skip(0), _context(nullptr), _error_addr(nullptr)
    {
    }

    size_t thread_id() const { return _thread_id; }

    void skip_n_firsts(size_t n) { _skip = n; }

  protected:
    // 获取线程id，如果是主线程，则隐藏id细节
    void load_thread_info()
    {
#ifdef BACKWARD_SYSTEM_LINUX
#ifndef __ANDROID__
        _thread_id = static_cast<size_t>(syscall(SYS_gettid));
#else
        _thread_id = static_cast<size_t>(gettid());
#endif
        if (_thread_id == static_cast<size_t>(getpid()))
        {
            // If the thread is the main one, let's hide that.
            // I like to keep little secret sometimes.
            _thread_id = 0;
        }
#endif
    }

    // 设置上下文
    void set_context(void *context) { _context = context; }
    // 获取上下文
    void *context() const { return _context; }

    // 设置寄存器地址
    void set_error_addr(void *error_addr) { _error_addr = error_addr; }
    // 获取寄存器地址
    void *error_addr() const { return _error_addr; }

    // 获取 _skip
    size_t skip_n_firsts() const { return _skip; }

  private:
    size_t _thread_id;
    size_t _skip;
    void *_context;
    void *_error_addr;
};

class StackTraceImplHolder : public StackTraceImplBase
{
  public:
    size_t size() const
    {
        return (_stacktrace.size() >= skip_n_firsts())
                   ? _stacktrace.size() - skip_n_firsts()
                   : 0;
    }
    Trace operator[](size_t idx) const
    {
        if (idx >= size())
        {
            return Trace();
        }
        return Trace(_stacktrace[idx + skip_n_firsts()], idx);
    }
    void *const *begin() const
    {
        if (size())
        {
            return &_stacktrace[skip_n_firsts()];
        }
        return nullptr;
    }

  protected:
    std::vector<void *> _stacktrace;
};

template <>
class StackTraceImpl<system_tag::current_tag> : public StackTraceImplHolder
{
  public:
    NOINLINE
    size_t load_here(size_t depth = 32, void *context = nullptr,
                     void *error_addr = nullptr)
    {
        load_thread_info();
        set_context(context);
        set_error_addr(error_addr);
        if (depth == 0)
        {
            return 0;
        }
        _stacktrace.resize(depth);
        size_t trace_cnt = details::unwind(callback(*this), depth);
        _stacktrace.resize(trace_cnt);
        skip_n_firsts(0);
        return size();
    }
    size_t load_from(void *addr, size_t depth = 32, void *context = nullptr,
                     void *error_addr = nullptr)
    {
        load_here(depth + 8, context, error_addr);

        for (size_t i = 0; i < _stacktrace.size(); ++i)
        {
            if (_stacktrace[i] == addr)
            {
                skip_n_firsts(i);
                break;
            }
        }

        _stacktrace.resize(
            std::min(_stacktrace.size(), skip_n_firsts() + depth));
        return size();
    }

  private:
    struct callback
    {
        StackTraceImpl &self;
        callback(StackTraceImpl &_self) : self(_self) {}

        void operator()(size_t idx, void *addr)
        {
            self._stacktrace[idx] = addr;
        }
    };
};

class StackTrace : public StackTraceImpl<system_tag::current_tag>
{
};

template <class T> class Handler
{
  private:
    struct dummy
    {
        void nonull() {}
    };
    T var_;
    bool empty_;

  public:
    Handler() : var_(), empty_(true) {}
    Handler(T val) : var_(val), empty_(false)
    {
        if (!var_)
        {
            empty_ = true;
        }
    }

    void reset(const T &new_val)
    {
        Handler tmp(new_val);
        swap(tmp);
    }

    T get() { return var_; }

    // 运算符重载
    T &operator->() { return var_; }
    const T &operator->() const { return var_; }

    typedef typename rm_ptr<T>::type &ref_t;
    typedef const typename rm_ptr<T>::type &const_ref_t;
    ref_t operator*() { return *var_; }
    const_ref_t operator*() const { return *var_; }
    ref_t operator[](size_t idx) { return var_[idx]; }

    Handler &operator=(Handler &&from)
    {
        swap(from);
        return *this;
    }
    // 实现if中判空。
    // 这里的实现逻辑其实很简单，dummy就是一个随便定义的类型，或者这里operator转成其他类型都可以实现if
    // 判空 因为在if 中会调用一次这个转换以便于与0或者nullptr判等。
    operator const dummy *() const
    {
        if (empty_)
        {
            return nullptr;
        }
        return reinterpret_cast<const dummy *>(var_);
    }
    // 包括转换成bool也可以实现 if判空
    // operator bool() {
    //     if (empty_) {
    //         return false;
    //     }
    //     return true;
    // }

  private:
    void swap(Handler &lfh)
    {
        using std::swap;
        swap(lfh.var_, var_);
        swap(lfh.empty_, empty_);
    }
};

class SignalHandling
{
  public:
    static std::vector<int> make_default_signals()
    {
        const int posix_signals[] = {
            // Signals for which the default action is "Core".
            SIGABRT, // Abort signal from abort(3)
            SIGBUS,  // Bus error (bad memory access)
            SIGFPE,  // Floating point exception
            SIGILL,  // Illegal Instruction
            SIGIOT,  // IOT trap. A synonym for SIGABRT
            SIGQUIT, // Quit from keyboard
            SIGSEGV, // Invalid memory reference
            SIGSYS,  // Bad argument to routine (SVr4)
            SIGTRAP, // Trace/breakpoint trap
            SIGXCPU, // CPU time limit exceeded (4.2BSD)
            SIGXFSZ, // File size limit exceeded (4.2BSD)
        };
        return std::vector<int>(posix_signals,
                                posix_signals + sizeof posix_signals /
                                                    sizeof posix_signals[0]);
    }

    SignalHandling(
        const std::vector<int> &posix_signals = make_default_signals())
        : loaded_(false)
    {
        bool success = true;
        size_t stack_size = 1024 * 1024 * 8;
        // todo 总结C++四种安全转化的区别
        stack_content_.reset(static_cast<char *>(malloc(stack_size)));
        if (stack_content_)
        {
            stack_t ss;
            ss.ss_sp = stack_content_.get();
            ss.ss_size = stack_size;
            ss.ss_flags = 0;
            // sigaltstack - set and/or get signal stack context
            // 设置栈溢出时备用栈,或者可以在后续设置信号处理函数运行在此栈上
            if (sigaltstack(&ss, nullptr) < 0)
            {
                success = false;
            }
        }
        else
        {
            success = false;
        }
        // 设置所有信号对应的处理函数
        for (auto signal : posix_signals)
        {
            struct sigaction action;
            memset(&action, 0, sizeof action);
            action.sa_flags = static_cast<int>(SA_SIGINFO | SA_ONSTACK |
                                               SA_NODEFER | SA_RESETHAND);
            // SA_NODEFER
            // 在信号处理函数执行时，不能将该信号添加进sa_mask中阻塞该信号，除非这个信号已经在sa_mask中被设置过了。信号阻塞相关概念：https://www.cnblogs.com/black-mamba/p/6876320.html
            // SA_ONSTACK 设置信号处理函数运行在备用栈上
            // SA_RESETHAND
            // 在进入执行信号处理函数时，将信号恢复默认值。即：重置信号
            // SA_SIGINFO The  signal  handler  takes  three arguments, not one.
            // In this case, sa_sigaction should be set instead of sa_handler.
            sigfillset(&action.sa_mask); // initializes set to full, including
                                         // all signals.
            sigdelset(&action.sa_mask, signal);
            // 设置信号处理函数
            action.sa_sigaction = &sig_handler;

            int res = sigaction(signal, &action, nullptr);
            if (res < 0)
            {
                success = false;
            }
        }
    }

  private:
    static void handleSignal(int signo, siginfo_t *info, void *_ctx)
    {
        ucontext_t *uctx = static_cast<ucontext_t *>(_ctx);
        StackTrace st;
        void *error_addr = nullptr;
        // gregs:Container for all general registers.
        // REG_RIP:Number of each register in the `gregset_t' array.编号
        // REG_RIP对应x86_64架构
#ifdef REG_RIP
        error_addr = reinterpret_cast<void *>(uctx->uc_mcontext.gregs[REG_RIP]);
#endif
        if (error_addr)
        {
            st.load_from(error_addr, 32, reinterpret_cast<void *>(uctx),
                         info->si_addr);
        }
        else
        {
            st.load_here(32, reinterpret_cast<void *>(uctx), info->si_addr);
        }
    }

    // 为什么设置为static的？
    // 因为在给sig_actiong赋值的时候C++的成员函数指针是直接赋值不进去的，因此转为static的
    static void sig_handler(int signo, siginfo_t *info, void *_ctx)
    {
        // 解析 _ctx 栈的上下文信息
        handleSignal(signo, info, _ctx);

        // raise - send a signal to the caller
        raise(info->si_signo);

        // stop the process imediately
        puts("what? exit");
        _exit(-1);
    }

  private:
    Handler<char *> stack_content_;
    bool loaded_;
};

#endif