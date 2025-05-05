#pragma once
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <mutex>

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    }
    pthread_mutex_t *Get()
    {
        return &_mutex;
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&_mutex);
    }

private:
    pthread_mutex_t _mutex;
};

class global_mutex
{
public:
    global_mutex(Mutex &mutex)
        : _mutex(mutex)
    {
        _mutex.Lock();
    }
    ~global_mutex()
    {
        _mutex.UnLock();
    }
private:
    Mutex &_mutex;
};