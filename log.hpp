#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include "mutex.hpp"

namespace LogModule
{
    class LogStrategy
    {
    public:
        ~LogStrategy() = default;
        virtual void SyncLog(const std::string &messege) = 0; // 表示纯虚函数，基类中不提供方法
    };

    const std::string gsep = "\r\n"; // separator分隔符

    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy()
        {
        }
        void SyncLog(const std::string &messege) override
        {
            global_mutex glock(_mutex); // 加锁保证多线程打印的原子性
            std::cout << messege << gsep;
        }
        ~ConsoleLogStrategy()
        {
        }

    private:
        Mutex _mutex;
    };

    const std::string DefaultPath = "./";       // 默认路径
    const std::string DefaultFile = "http.log"; // 默认日志文件名
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string path = DefaultPath, const std::string file = DefaultFile)
            : _path(path),
              _file(file)
        {
        }
        void SyncLog(const std::string &messege) override
        {
            global_mutex glock(_mutex);

            std::string filename = _path + (_path.back() == '/' ? "" : "/") + _file; // 将路径和文件名拼成完整的文件名
            std::ofstream out(filename, std::ios::app);                              // 以追加打开文件
            if (!out.is_open())
                return;
            out << messege << gsep;
            out.close();
        }
        ~FileLogStrategy()
        {
        }

    private:
        std::string _path;
        std::string _file;
        Mutex _mutex;
    };

    enum class LogLevel // 定义日志等级
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    std::string Level2Str(LogLevel loglevel) // 获取日志等级字符串
    {
        switch (loglevel)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNDEF"; // undef
        }
    }

    std::string GetTimeStamp()
    {
        time_t curr = time(nullptr);
        struct tm curr_tm;
        localtime_r(&curr, &curr_tm);
        char res[128];
        snprintf(res, sizeof(res), "%04d-%02d-%02d %02d:%02d:%02d",
                 curr_tm.tm_year + 1900, // 是从1900年开始算的
                 curr_tm.tm_mon + 1,     // [0,11] 表示12个月份
                 curr_tm.tm_mday,
                 curr_tm.tm_hour,
                 curr_tm.tm_min,
                 curr_tm.tm_sec);
        return res;
    }
    class Logger
    {
    public:
        Logger()
        {
            EnableConsoleLogStrategy();
        }
        void EnableConsoleLogStrategy()
        {
            _fflush_strategy = std::make_unique<ConsoleLogStrategy>();
        }
        void EnableFileLogStrategy()
        {
            _fflush_strategy = std::make_unique<FileLogStrategy>();
        }

        class LogMessege // 在这个类里面生成一条string的日志内容，再析构时刷新到目标位置
        {
        public:
            LogMessege(LogLevel level, std::string file, int line, Logger &logger)
                : _time(GetTimeStamp()),
                  _level(level),
                  _pid(getpid()),
                  _file(file),
                  _line(line),
                  _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _time << "]"
                   << "[" << Level2Str(_level) << "]"
                   << "[" << _pid << "]"
                   << "[" << _file << "]"
                   << "[" << _line << "]"
                   << "-";

                _loginfo = ss.str();
            }
            template <class T>
            LogMessege &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }
            ~LogMessege()
            {
                if (_logger._fflush_strategy) // if指针不为空
                    _logger._fflush_strategy->SyncLog(_loginfo);
            }

        private:
            std::string _time;
            LogLevel _level;
            pid_t _pid;
            std::string _file;
            int _line;
            std::string _loginfo; // 合并后完整的日志，传到策略类里面刷新
            Logger &_logger;
        };

        LogMessege operator()(LogLevel level, std::string file, int line)
        {
            return LogMessege(level, file, line, *this); // 返回临时对象，返回后自动析构，刷新
        }

    private:
        std::unique_ptr<LogStrategy> _fflush_strategy;
    };

    Logger log; // 全局对象

#define LOG(level) log(level, __FILE__, __LINE__) // 调用operator()构造内部类LogMessege
#define ConsoleLogStrategy() log.EnableConsoleLogStrategy()
#define FileLogStrategy() log.EnableFileLogStrategy();
}
