#pragma once
#include "Common.hpp"
#include <sys/epoll.h>
using namespace LogModule;

class Epoller // epoll模型
{
public:
    Epoller()
    {
        _epfd = epoll_create(128);
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error";
            exit(CREATE_EPOLL_ERR);
        }
    }
    ~Epoller()
    {
        if (_epfd > 0)
            close(_epfd);
    }
    void ModEventHelper(int sockfd, uint32_t events, int oper)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sockfd; // 后面会用这个fd从hash里面反查表得到Connection
        int n = epoll_ctl(_epfd, oper, sockfd, &ev);
        if (n < 0)
        {
            LOG(LogLevel::WARNING) << "epoll_ctl error";
            return;
        }
        LOG(LogLevel::INFO) << "epoll_ctl success";
    }
    void ModEvent(int sockfd, uint32_t events)
    {
        ModEventHelper(sockfd, events, EPOLL_CTL_MOD);
    }
    void AddEvent(int sockfd, uint32_t events)
    {
        ModEventHelper(sockfd, events, EPOLL_CTL_ADD);
    }
    void DelEvent(int sockfd)
    {
        int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
    }
    int WaitEvent(struct epoll_event revs[], int maxnum, int timeout)
    {
        int n = epoll_wait(_epfd, revs, maxnum, timeout);
        if (n < 0)
            LOG(LogLevel::WARNING) << "epoll_wait error";
        else if (n == 0)
            LOG(LogLevel::INFO) << "epoll_wait timeout";
        return n;
    }

private:
    int _epfd;
};
