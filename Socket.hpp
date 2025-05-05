#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.hpp"
#include "log.hpp"
#include "InetAddr.hpp"
#include "Tool.hpp"

using namespace LogModule;

class Socket
{
public:
    ~Socket()
    {
    }
    virtual void SocketOrDie() = 0; // 纯虚函数，等子类实现
    virtual void BindOrDie(uint16_t port) = 0;
    virtual void ListenOrDie(int backlog) = 0;
    virtual std::shared_ptr<Socket> Accept(InetAddr *client) = 0;
    virtual void Close() = 0;
    virtual int Send(const std::string message) = 0;
    virtual int Recv(std::string *out) = 0;
    virtual int Connect(InetAddr &server) = 0;

public:
    void BuildTcpSocket(uint16_t port, int backlog = 16)
    {
        SocketOrDie();
        BindOrDie(port);
        ListenOrDie(backlog);
    }
    void BuildTcpSocketForClient()
    {
        SocketOrDie();
    }

private:
};

const static int defaultfd = -1;

class TcpSocket : public Socket
{
public:
    TcpSocket() // 无参构造
        : _sockfd(defaultfd)
    {
    }
    TcpSocket(int fd)
        : _sockfd(fd)
    {
    }
    ~TcpSocket() {}
    void SocketOrDie() override
    {
        _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket success";
    }
    void BindOrDie(uint16_t port) override
    {
        InetAddr localaddr(port);
        int n = ::bind(_sockfd, localaddr.NetAddrPtr(), localaddr.AddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success";
    }
    void ListenOrDie(int backlog) override
    {
        int n = ::listen(_sockfd, backlog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen success";
    }
    std::shared_ptr<Socket> Accept(InetAddr *client) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int fd = ::accept(_sockfd, CONV(peer), &len);
        // std::cout << std::to_string(peer.sin_addr.s_addr) << std::endl;
        if (fd < 0)
        {
            LOG(LogLevel::FATAL) << "accept error";
            exit(ACCEPT_ERR);
        }
        client->SetAddr(peer);
        return std::make_shared<TcpSocket>(fd);
    }
    void Close() override
    {
        if (_sockfd > 0)
            ::close(_sockfd);
    }
    // 返回值就是send的返回值 同wirte
    int Send(const std::string message) override
    {
        return ::send(_sockfd, message.c_str(), message.size(), 0); // 和write是一样的
    }
    int Recv(std::string *out)
    {
        char buffer[1024];
        int n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0); // 和read是一样的
        if (n > 0)
        {
            buffer[n] = 0;
            *out += buffer; // 每次收到的消息都让它追加在后面
        }
        return n;
    }
    int Connect(InetAddr &server) override
    {
        // 客户端连接服务器
        return ::connect(_sockfd, server.NetAddrPtr(), server.AddrLen());
    }

private:
    int _sockfd; // 可以是监听端口，也可以是读写端口
};