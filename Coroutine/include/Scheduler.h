#include <memory>
#include <string>
#include <list>
#include <atomic>

#include "Fiber.h"
#include "mutex.h"

#ifndef __MY_SCHEDULER__
#define __MY_SCHEDULER__

namespace Coroutine {


/**
 * 协程调度器
 * 封装的是N-M的协程调度器，内部有一个线程池，支持协程在线程池里面切换
*/
class Scheduler {
public:
    using ptr = std::shared_ptr<Scheduler>;
    using MutexType = Mutex;

    /**
     * 创建调度器
     * threads:线程数
     * use_caller:是否将当前线程也作为调度线程
     * name:名称
    */
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "Scheduler");
    virtual ~Scheduler();

    // 获取调度器名称
    const std::string& getName() const { return m_name_; }

    /**
     * @brief 获取当前线程调度器指针
    */
    static Scheduler* GetThis();

    /**
     * @brief 获取当前线程的主协程
    */
    static Fiber* GetMainFiber();

    /**
     * @brief 添加调度任务
     * @tparam FiberIrCb 调度任务类型，可以是协程对象或函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号， -1 表示任意线程
    */
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex_);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if (need_tickle) {
            tickle(); // 唤醒idle协程
        }
    }

    void start();
    void stop();
protected:
    // 通知协程调度器有任务了
    virtual void tickle();
    // 协程调度函数
    void run();
    // 无任务调度时执行idle协程
    virtual void idle();
    // 返回是否可以停止
    virtual bool stopping();
    // 设置当前的协程调度器
    void SetThis();
    /**
     * @brief 返回是否有空闲线程
     * @details 当调度协程进入idle时空闲线程数+1， 从idle协程返回时空闲线程数-1
    */
    bool hasIdleThreads() { return m_idleThreadCount_ > 0; }
private:
    struct ScheduleTask
    {
        Fiber::ptr fiber_;
        std::function<void()> cb_;
        int thread_;
        ScheduleTask(Fiber::ptr f, int thr) {
            fiber_ = f;
            thread_ = thr;
        }
        ScheduleTask(Fiber::ptr *f, int thr) {
            fiber_.swap(*f);
            thread_ = thr;
        }
        ScheduleTask(std::function<void()> f, int thr) {
            cb_ = f;
            thread_ = thr;
        }
        ScheduleTask() { thread_ = -1; }

        void reset() {
            fiber_ = nullptr;
            cb_ = nullptr;
            thread_ = -1;
        }
    };

private:
    std::string m_name_;
    MutexType m_mutex_;
    // 线程池
    std::vector<Thread::ptr> m_threads_;
    // 任务队列
    std::list<ScheduleTask> m_stasks_;
    // 线程池的线程ID数组
    std::vector<int> m_threadsIds_;
    // 工作线程数量， 不包含use_caller的主线程
    size_t m_threadCount_ = 0;
    // 活跃线程数
    std::atomic<size_t> m_activeThreadCount_ = 0;
    // idle 线程数量
    std::atomic<size_t> m_idleThreadCount_ = 0;

    // 是否 use caller
    bool m_useCaller_;
    // use_caller 为true时，调度器所在线程的调度协程
    Fiber::ptr m_rootFiber_;
    // use_caller 为true时，调度器所在线程的id
    int m_rootThread = 0;
    int m_stopping_ = false;
};

} // end of namespace Coroutine

#endif