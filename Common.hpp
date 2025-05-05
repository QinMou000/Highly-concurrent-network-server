#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    CONNECT_ERR
};

#define CONV(addr) (struct sockaddr*)(&addr)