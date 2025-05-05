#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.hpp"

class InetAddr
{
public:
    InetAddr()
    {
    }
    InetAddr(struct sockaddr_in &addr)
    {
        SetAddr(addr);
    }
    InetAddr(std::string &ip, uint16_t port)
        : _ip(ip),
          _port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr);
        _addr.sin_port = htons(_port); // 从主机字节序，转为网络字节序
    }
    InetAddr(uint16_t port) // 服务端
        : _port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY; // 一个主机可能有多个网卡，不用绑定特定ip，0.0.0.0就是默认监听所有可用ip
        _addr.sin_port = htons(_port);      // 从主机字节序，转为网络字节序
    }
    void SetAddr(struct sockaddr_in &addr)
    {
        _addr = addr;
        // 网络转主机
        _port = ntohs(_addr.sin_port);   // 从网络中拿到的 网络序列
        _ip = inet_ntoa(_addr.sin_addr); // 4字节网络风格的IP -> 点分十进制的字符串风格的IP
    }
    uint16_t Port() { return _port; }
    std::string Ip() { return _ip; }
    std::string StringAddr()
    {
        return _ip + " : " + std::to_string(_port);
    }
    struct sockaddr *NetAddrPtr()
    {
        return CONV(_addr);
    }
    socklen_t AddrLen()
    {
        return sizeof(_addr);
    }
    bool operator==(const InetAddr &addr)
    {
        return _port == addr._port && _ip == addr._ip;
    }
    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};