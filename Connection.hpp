#pragma once
#include <iostream>
#include <functional>
#include "Reactor.hpp"

using hander_t = std::function<std::string(std::string &)>; // 参数故意写成引用

class Reactor;

class Connection // 连接基类
{
public:
    Connection() {}
    ~Connection() {}

    // 子类们就必须实现各自的读写操作
    virtual void Recver() = 0;   // 处理读操作
    virtual void Sender() = 0;   // 处理写操作
    virtual void Excepter() = 0; // 处理异常操作
    virtual int GetSockfd() = 0;

    uint32_t GetEvent()
    {
        return _events;
    }
    void SetEvent(const uint32_t events)
    {
        _events = events;
    }
    void SetOwner(Reactor *owner)
    {
        _owner = owner;
    }
    Reactor *GetOwner()
    {
        return _owner;
    }
    void SetHander(hander_t hander)
    {
        _hander = hander;
    }

private:
    uint32_t _events; // 要关心的时间 会被每一个连接继承
    Reactor *_owner;  // 回指指针 也会被每一个连接继承

public:
    hander_t _hander;
};