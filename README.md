# Personal Web Server

一个基于C++的高性能网络服务器，采用Reactor模式实现，支持HTTP协议。

## 功能特性

- 基于Reactor模式的异步I/O处理
- 支持HTTP/1.1协议
- 使用epoll实现高效的事件驱动
- 支持多连接并发处理
- 内置日志系统
- 支持守护进程模式运行
- 可配置的静态文件服务

## 项目结构

```
.
├── lib/                # 第三方库
├── include/           # 头文件
├── wwwroot/          # 静态文件目录
├── Channel.hpp       # 事件通道封装
├── Connection.hpp    # 连接管理
├── Epoller.hpp       # epoll封装
├── Http.hpp          # HTTP协议实现
├── InetAddr.hpp      # 网络地址封装
├── Listener.hpp      # 监听器实现
├── Reactor.hpp       # Reactor核心实现
├── Socket.hpp        # Socket封装
├── Tcpserver.hpp     # TCP服务器实现
├── Tool.hpp          # 工具函数
├── daemon.hpp        # 守护进程实现
├── log.hpp           # 日志系统
├── mutex.hpp         # 互斥锁封装
└── Main.cc           # 主程序入口
```

## 构建要求

- C++11或更高版本
- Linux操作系统
- Make工具
- GCC编译器

## 构建步骤

1. 克隆仓库
```bash
git clone [repository-url]
cd personal-web
```

2. 编译项目
```bash
make
```

3. 运行服务器
```bash
./server
```

## 配置说明

服务器默认监听8080端口，可以通过修改配置来更改端口号和其他参数。

## 使用示例

1. 启动服务器：
```bash
./server
```

2. 访问服务：
```bash
curl http://localhost:8080
```

## 日志

服务器运行日志保存在`http.log`文件中，可以通过查看该文件来监控服务器运行状态。

## 注意事项

- 确保wwwroot目录具有适当的访问权限
- 建议在生产环境中使用守护进程模式运行
- 请确保系统有足够的文件描述符限制


## 贡献

欢迎提交Issue和Pull Request来帮助改进项目。 