// #include "Simple_Fiber_Scheduler.h"
#include "../include/Simple_Fiber_Scheduler.h"
// #include <unistd.h>

namespace Coroutine {

void Scheduler::Schedule(Coroutine::Fiber::ptr task) {
    m_tasks_.push_back(task);
}

void Scheduler::Run() {
    Coroutine::Fiber::ptr task;
    auto it = m_tasks_.begin();
    while (it != m_tasks_.end())
    {
        // std::cout << "<< 1 " << task.use_count() << std::endl;
        task = *it;
        m_tasks_.erase(it ++);
        // std::cout << "<< 2 " << task.use_count() << std::endl;
        task->Resume();
    }
}
} // end of namespace Coroutine