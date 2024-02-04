#ifndef __MY_NONCOPYABLE_
#define __MY_NONCOPYABLE_
namespace Coroutine {

class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};


} // end of namespace Coroutine

#endif