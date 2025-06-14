#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <functional>
#include <sys/wait.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include "log.hpp"

class NoCopy
{
public:
    NoCopy() {}
    NoCopy(const NoCopy &) = delete;
    const NoCopy &operator=(const NoCopy &) = delete;
    ~NoCopy() {}
};

enum ExitCode
{
    OK = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    FORK_ERR,
    ACCEPT_ERR,
    CONNECT_ERR,
    CREATE_EPOLL_ERR
};

#define CONV(addr) (struct sockaddr *)(&addr)

void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
        return;
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}