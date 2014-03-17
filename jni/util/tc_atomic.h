#ifndef __TC_ATOMIC_H
#define __TC_ATOMIC_H

#include <stdint.h>
#include "util/tc_lock.h"
#include "util/tc_thread_mutex.h"

namespace taf
{

//#include "atomic_asm.h"

/////////////////////////////////////////////////
/** 
 * @file  tc_atomic.h 
 * @brief  原子计数类.
 *  
 * @author  jarodruan@tencent.com 
 */           
/////////////////////////////////////////////////

/**
 * @brief 原子操作类,对int做原子操作
 */
class TC_Atomic : public TC_ThreadMutex
{
public:

    /**
     * 原子类型
     */
    typedef int atomic_type;

    /**
     * @brief 构造函数,初始化为0
     */
    TC_Atomic(atomic_type at = 0)
    {
        set(at);
    }

    TC_Atomic& operator++()
    {
        inc();
        return *this;
    }

    TC_Atomic& operator--()
    {
        dec();
        return *this;
    }

    operator atomic_type() const
    {
        return get();
    }

    TC_Atomic& operator+=(atomic_type n)
    {
        add(n);
        return *this;
    }

    TC_Atomic& operator-=(atomic_type n)
    {
        sub(n);
        return *this;
    }

    TC_Atomic& operator=(atomic_type n)
    {
        set(n);
        return *this;
    }

    /**
     * @brief 获取值
     *
     * @return int
     */
    atomic_type get() const         { TC_LockT<TC_ThreadMutex> lock(_mutex); return _value; }

    /**
     * @brief 添加
     * @param i
     *
     * @return int
     */
    atomic_type add(atomic_type i)  { TC_LockT<TC_ThreadMutex> lock(_mutex); _value += 1; return _value;}

    /**
     * @brief 减少
     * @param i
     *
     * @return int
     */
    atomic_type sub(atomic_type i)  { TC_LockT<TC_ThreadMutex> lock(_mutex); _value -=1; return _value; }

    /**
     * @brief 自加1
     *
     * @return int
     */
    atomic_type inc()               { return add(1); }

    /**
     * @brief 自减1
     */
    atomic_type dec()               { return sub(1); }

    /**
     * @brief 自加1
     *
     * @return void
     */
    void inc_fast()               	{ TC_LockT<TC_ThreadMutex> lock(_mutex); _value += 1; }

    /**
     * @brief 自减1
     * Atomically decrements @_value by 1 and returns true if the
     * result is 0, or false for all other
     */
    bool dec_and_test()             { TC_LockT<TC_ThreadMutex> lock(_mutex); _value -= 1; return _value==0; }

    /**
     * @brief 设置值
     */
    atomic_type set(atomic_type i)  { TC_LockT<TC_ThreadMutex> lock(_mutex); _value = i; return i; }

protected:

    /**
     * 锁
     * 
     */
    TC_ThreadMutex  _mutex;

    /**
     * 值
     */
    atomic_type     _value;
};

}

#endif
