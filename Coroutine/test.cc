#include "./include/Fiber.h"
#include "./include/Simple_Fiber_Scheduler.h"
#include <iostream>


void test_fiber(int i) {
    std::cout << "hello world" << i << std::endl;
}

int main() {
    Coroutine::Fiber::GetThis();

    Coroutine::Scheduler sc;

    for (int i = 0; i < 10; ++ i) {
        Coroutine::Fiber::ptr fiber(new Coroutine::Fiber(std::bind(test_fiber, i)));
        sc.Schedule(fiber);
    }

    sc.Run();
    return 0;
}