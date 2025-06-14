#pragma once
#include <memory>
#include "Common.hpp"
#include "Epoller.hpp"
#include "Connection.hpp"

using namespace LogModule;

class Reactor // 反应堆容器
{
    static const int _revs_num = 128;

private:
    bool IsConExistsHelper(int sockfd)
    {
        if (_connection.find(sockfd) == _connection.end())
            return false;
        return true;
    }
    bool IsConExists(std::shared_ptr<Connection> &con)
    {
        return IsConExistsHelper(con->GetSockfd());
    }
    bool IsConExists(int sockfd)
    {
        return IsConExistsHelper(sockfd);
    }
    int LoopOnce(int timeout)
    {
        return _epoll->WaitEvent(_revs, _revs_num, timeout);
    }

    void Dispatcher(int n)
    {
        // LOG(LogLevel::DEBUG) << n << "个事件就绪了";
        for (int i = 0; i < n; i++) // Epoll模型下有n个fd关系的事件就绪了，它会自动把这几个就绪的fd和event放在最前面n个位置
        {
            int sockfd = _revs[i].data.fd;     // 就绪的fd
            uint32_t revent = _revs[i].events; // 就绪的事件

            if (revent & EPOLLERR)
                revent |= (EPOLLIN | EPOLLOUT); // 将错误的情况全部重定位到recver和sender逻辑里面
            if (revent & EPOLLHUP)
                revent |= (EPOLLIN | EPOLLOUT); // 将错误的情况全部重定位到recver和sender逻辑里面

            if (revent & EPOLLIN)
                if (IsConExists(sockfd)) // 如果当前链接存在 那就调用当前连接的接收逻辑
                    _connection[sockfd]->Recver();
            if (revent & EPOLLOUT)
                if (IsConExists(sockfd)) // 如果当前链接存在 那就调用当前连接的发送逻辑
                    _connection[sockfd]->Sender();
        }
    }

public:
    Reactor()
        : _epoll(std::make_unique<Epoller>()),
          _isrunning(false)
    {
    }
    void Loop()
    {
        _isrunning = true;
        int timeout = -1; // 设置为阻塞
        while (_isrunning)
        {
            // PrintFd();
            int n = LoopOnce(timeout);
            Dispatcher(n); // 将返回值（有多少个事件就绪了）传给事件派发器
        }
        _isrunning = false;
    }
    void AddConnection(std::shared_ptr<Connection> &con)
    {
        // 避免重复添加
        if (IsConExists(con))
        {
            LOG(LogLevel::WARNING) << "con is exists: " << con->GetSockfd();
            return;
        }
        // 将连接加入epoll模型，并写透到内核
        int sockfd = con->GetSockfd();
        uint32_t events = con->GetEvent();
        _epoll->AddEvent(sockfd, events);

        // 设置回指指针
        con->SetOwner(this);

        // 添加{fd, con}到hash里面
        _connection[sockfd] = con;
    }
    void EnableReadWrite(int sockfd, bool read, bool write)
    {
        if (!IsConExists(sockfd)) // 保证存在这个连接
            return;

        // 修改connecction里面对fd的事件关心
        uint32_t newevent = (EPOLLET | (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0));
        _connection[sockfd]->SetEvent(newevent);

        // 将这个事件写透到内核 改变关心的事件
        _epoll->ModEvent(sockfd, newevent);
    }
    void DelConnection(int sockfd)
    {
        // 从epoll中删除的fd必须是合法的
        _epoll->DelEvent(sockfd);

        // 从hash里面删除
        _connection.erase(sockfd);

        // 关闭文件描述符
        close(sockfd);

        LOG(LogLevel::INFO) << "client quit!";
    }
    void Stop()
    {
        _isrunning = false;
    }
    void PrintFd()
    {
        for (auto con : _connection)
            std::cout << con.first;
        std::cout << std::endl;
    }
    ~Reactor() {}

private:
    std::unique_ptr<Epoller> _epoll;                                  // epoll模型
    std::unordered_map<int, std::shared_ptr<Connection>> _connection; // 管理所有连接的哈希表 用fd来做索引
    struct epoll_event _revs[_revs_num];                              // 管理epoll要关心的事件 {fd, event}
    bool _isrunning;
};