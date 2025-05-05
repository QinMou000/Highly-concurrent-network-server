#pragma once
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include "log.hpp"

const std::string null_dev = "/dev/null";
using namespace LogModule;
// 守护进程
void daemon()
{
    // 1. 忽略IO，子进程退出信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // 2. 新起一个进程，并退出父进程
    if (fork() > 0)
        exit(1);

    // 3. 新建一个会话
    setsid();

    // 4. 更改进程的当前执行路径
    // chdir("/");

    // 5. 关闭标准输入输出错误 可能导致本来输出到显示器的东西出错
    // 建议重定向到 /dev/null 文件里（无底洞）
    int fd = ::open(null_dev.c_str(), O_RDWR);
    if (fd < 0)
        LOG(LogLevel::FATAL) << "open dev/null error";
    else
    {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    }
}