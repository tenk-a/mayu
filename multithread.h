//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// multithread.h
// マルチスレッド用. (実質ダミー)

#ifndef _MULTITHREAD_H
#define _MULTITHREAD_H

#if 0 //defined(WIN32)
 #include <windows.h>
#elif defined(__linux__)
#elif defined(__APPLE__)
#endif

///
class SyncObject
{
public:
    ///
    virtual void    acquire()  = 0;

    ///
    virtual void acquire(int ) { acquire(); }
    ///
    virtual void    release()  = 0;
};

# if 0 //defined(_MSC_VER)
///
class CriticalSection : public SyncObject
{
    CRITICAL_SECTION m_cs;  ///<

public:
    ///
    CriticalSection() { InitializeCriticalSection(&m_cs); }
    ///
    ~CriticalSection() { DeleteCriticalSection(&m_cs); }
    ///
    void acquire() { EnterCriticalSection(&m_cs); }
    ///
    void release() { LeaveCriticalSection(&m_cs); }
};

#elif defined(__linux__) || defined(__APPLE__)
// TODO:

// とりあえずシングルスレッドで動かすので Null Object
class CriticalSection : public SyncObject
{
public:
    ///
    CriticalSection() { }
    ///
    virtual ~CriticalSection() { }
    ///
    void acquire() { }
    ///
    void release() { }
};

class Mutex : public SyncObject
{
public:
    ///
    Mutex() {}
    ///
    virtual ~Mutex() {}
    ///
    void acquire() {}
    ///
    void release() {}
};

#endif

///
class Acquire
{
    SyncObject *m_so;       ///

public:
    ///
    Acquire(SyncObject *i_so) : m_so(i_so) { m_so->acquire(); }
    ///
    Acquire(SyncObject *i_so, int i_n) : m_so(i_so) { m_so->acquire(i_n); }
    ///
    ~Acquire() { m_so->release(); }
};


#endif // !_MULTITHREAD_H
