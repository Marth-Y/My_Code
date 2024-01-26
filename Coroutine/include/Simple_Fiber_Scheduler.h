#include <list>

#include "debug.h"
#include "Fiber.h"   

#ifndef __SIMPLE__SCHEDULER__
#define __SIMPLE__SCHEDULER__

namespace Coroutine {
class Scheduler {
public:
    void Schedule(Coroutine::Fiber::ptr task);
    void Run();

private:
    // 协程队列
    std::list<Coroutine::Fiber::ptr> m_tasks_;

};
}; // end of namespace Coroutine

#endif

