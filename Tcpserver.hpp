#pragma once
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>
#include <sys/wait.h>
#include "log.hpp"
#include "Common.hpp"
#include "Socket.hpp"

using server_t = std::function<void(std::shared_ptr<Socket> &sock, InetAddr &addr)>;

using namespace LogModule;

class TcpServer : public NoCopy
{
public:
    TcpServer(uint16_t port, server_t server)
        : _listensocket(std::make_unique<TcpSocket>()),
          _port(port),
          _isrunning(false),
          _server(server)
    {
        _listensocket->BuildTcpSocket(_port);
    }
    void Start()
    {
        _isrunning = true;
        while (1)
        {
            InetAddr client;
            auto sock = _listensocket->Accept(&client);
            if (sock == nullptr)
                continue;
            LOG(LogLevel::DEBUG) << "Accept success";

            pid_t id = fork();
            if (id < 0)
            {
                LOG(LogLevel::FATAL) << "fork error";
                exit(FORK_ERR);
            }
            else if (id == 0) // 子进程
            {
                _listensocket->Close();
                if (fork() > 0) // 子进程直接退，留下孙子进程（孤儿进程也就被1号进程领养了）没有阻塞等待问题
                    exit(OK);
                _server(sock, client); // 外面传的server函数就可以拿着一个sock（从哪里读数据）client（数据从哪里发的）完成任务了
                sock->Close();
                exit(OK);
            }
            else // 父进程
            {
                sock->Close();
                pid_t rid = waitpid(id, nullptr, 0);
                (void)rid;
            }
        }
        _isrunning = false;
    }
    ~TcpServer()
    {
    }

private:
    std::unique_ptr<Socket> _listensocket; // 用这个套接字专门来供父进程监听
    uint16_t _port;
    bool _isrunning;
    server_t _server; // 外面传的server函数，可以是上层调用
};