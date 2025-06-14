#pragma once

#include <iostream>
#include "Connection.hpp"
#include "InetAddr.hpp"

class Channel : public Connection
{
public:
    Channel(int sockfd, InetAddr addr)
        : _sockfd(sockfd),
          _client_addr(addr)
    {
        SetNonBlock(_sockfd);
    }
    ~Channel() {}

    void Recver() override // 处理读操作
    {
        char buffer[1024];
        while (1)
        {
            int n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                _inbuffer += buffer; // 入缓冲区
            }
            else if (n == 0) // 对方关闭连接
            {
                Excepter();
                return;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) // 非阻塞模式下没有数据可读，退出循环
                    break;
                else if (errno == EINTR) // 系统调用被信号中断，忽略并继续读取。
                    continue;
                else // 真的读取错误 所有错误都归结到这个函数里面统一处理
                {
                    Excepter();
                    return;
                }
            }
        }
        LOG(LogLevel::INFO) << "client: " << _client_addr.StringAddr();

        // LOG(LogLevel::DEBUG) << "Channel inbuffer: \r\n"
        //                      << _inbuffer;

        if (!_inbuffer.empty()) // 将得到的数据交由上层处理 后返回到outbuffer中
            if (_hander)
                _outbuffer += _hander(_inbuffer);
            else
                LOG(LogLevel::DEBUG) << "bad callback"; // _hander没有初始化
        if (!_outbuffer.empty())
            Sender();
    }
    void Sender() override // 处理写操作
    {
        // 将发送缓冲区中的发送出去即可
        while (1)
        {
            int n = send(_sockfd, _outbuffer.c_str(), _outbuffer.size(), 0);
            if (n > 0)
            {
                _outbuffer.erase(0, n); // 删除outbuffer中的数据，避免重复发送
                if (_outbuffer.empty())
                    break; // 发完了就直接break
            }
            else if (n == 0) // 对方关闭连接
                break;       // TODO????
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) // 非阻塞模式下没有数据可读，退出循环
                    break;
                if (errno == EINTR) // 系统调用被信号中断，忽略并继续读取。
                    break;
                else // 真的读取错误 所有错误都归结到这个函数里面统一处理
                {
                    Excepter();
                    return;
                }
            }
        }

        // 到这里要么发送完了 要么对方的接受缓冲区被我们塞满了
        if (!_outbuffer.empty()) // 将写事件开启关心 在对方缓冲区有空间时再发
            GetOwner()->EnableReadWrite(_sockfd, true, true);
        else
            GetOwner()->EnableReadWrite(_sockfd, true, false);
    }
    void Excepter() override // 处理异常操作
    {
        GetOwner()->DelConnection(_sockfd); // 删除连接 取消事件关心
    }
    int GetSockfd() override
    {
        return _sockfd;
    }

private:
    int _sockfd;
    std::string _inbuffer;  // 输入缓冲区
    std::string _outbuffer; // 输出缓冲区
    InetAddr _client_addr;  // 对端信息
};