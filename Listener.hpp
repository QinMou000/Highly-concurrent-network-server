#pragma once
#include <iostream>
#include <memory>
#include "Common.hpp"
#include "Socket.hpp"
#include "Connection.hpp"
#include "Channel.hpp"

class Listener : public Connection
{

public:
    Listener(int port)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocket(_port);
        SetEvent(EPOLLIN | EPOLLET); // ET
        SetNonBlock(_listensock->Fd());
    }
    void Recver() override
    {
        InetAddr client;
        while (1)
        {
            int newfd = _listensock->Accept(&client);
            if (newfd == ACCEPT_ERR)
                break;
            else if (newfd == ACCEPT_CONTINUE)
                continue;
            else if (newfd == ACCEPT_DONE)
                break;
            else
            {
                // 构造新连接
                std::shared_ptr<Connection> con = std::make_shared<Channel>(newfd, client);
                // 设置新连接的事件
                con->SetEvent(EPOLLIN | EPOLLET);
                // 注册hander
                if (_hander != nullptr)      // 将listener基类中的_hander写到新连接里面
                    con->SetHander(_hander); // 注意不是写到listen里面而是写到con新连接里
                // 添加新连接到connection hash 里面并写透到内核
                GetOwner()->AddConnection(con);
            }
        }
    }
    void Sender() override
    {
    }
    void Excepter() override
    {
    }
    int GetSockfd() override
    {
        return _listensock->Fd();
    }
    ~Listener() {}

private:
    std::unique_ptr<Socket> _listensock;
    int _port;
};
