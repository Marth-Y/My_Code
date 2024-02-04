#include <pthread.h>
#include "noncopyable.h"

#ifndef __MY_MUTEX__
#define __MY_MUTEX__


namespace Coroutine {



/**
 * @brief 局部锁的模板实现
 */
template<class T>
struct ScopedLockImpl {
public:
    /**
     * @brief 构造函数
     * @param[in] mutex Mutex
     */
    ScopedLockImpl(T& mutex)
        :m_mutex_(mutex) {
        m_mutex_.lock();
        m_locked_ = true;
    }

    /**
     * @brief 析构函数,自动释放锁
     */
    ~ScopedLockImpl() {
        unlock();
    }

    /**
     * @brief 加锁
     */
    void lock() {
        if(!m_locked_) {
            m_mutex_.lock();
            m_locked_ = true;
        }
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        if(m_locked_) {
            m_mutex_.unlock();
            m_locked_ = false;
        }
    }
private:
    /// mutex
    T& m_mutex_;
    /// 是否已上锁
    bool m_locked_;
};


/**
 * @brief 互斥量
 */
class Mutex : Noncopyable {
public: 
    /// 局部锁
    typedef ScopedLockImpl<Mutex> Lock;

    /**
     * @brief 构造函数
     */
    Mutex() {
        pthread_mutex_init(&m_mutex_, nullptr);
    }

    /**
     * @brief 析构函数
     */
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex_);
    }

    /**
     * @brief 加锁
     */
    void lock() {
        pthread_mutex_lock(&m_mutex_);
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        pthread_mutex_unlock(&m_mutex_);
    }
private:
    /// mutex
    pthread_mutex_t m_mutex_;
};

} // end of namespace Coroutine

#endif 