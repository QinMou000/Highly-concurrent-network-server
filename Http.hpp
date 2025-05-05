#pragma once
#include "Common.hpp"
#include <unordered_map>
#include "Tool.hpp"

const std::string line_break = "\r\n"; // 换行符
const std::string blank = " ";         // 空格
const std::string webroot = "./wwwroot";
const std::string homepage = "index.html";
const std::string linesep = ": ";

class Request
{
public:
    Request() {}
    ~Request() {}
    // void Serialize() // 只做服务端，不必要
    // {

    // }
    void ParseReqline(std::string reqline)
    {
        std::stringstream ss(reqline);
        ss >> _method >> _uri >> _http_version; // 将请求行按空格输入到成员变量里
    }
    void Deserialize(std::string recv_str)
    {
        // 提取请求行
        std::string reqline;
        bool is_ok = Tool::GetOneLine(recv_str, &reqline, line_break);
        if (!is_ok)
            return;
        else
            ParseReqline(reqline); // 分析首行，写入_method  _uri  _http_version
        if (_uri == "/")
            _uri = webroot + _uri + homepage;
        else
            _uri = webroot + _uri;

        LOG(LogLevel::DEBUG) << "_method: " << _method;
        LOG(LogLevel::DEBUG) << "_uri: " << _uri;
        LOG(LogLevel::DEBUG) << "_http_version: " << _http_version;
    }
    std::string Uri()
    {
        return _uri;
    }

private:
    std::string _method;       // 请求方法
    std::string _uri;          // URI 请求资源路径
    std::string _http_version; // http版本

    std::unordered_map<std::string, std::string> _headers; // 请求报头

    std::string _text; // 请求正文
};

class Response
{
public:
    Response()
        : _http_version("HTTP/1.1")
    {
    }
    ~Response() {}
    std::string Serialize()
    {
        std::string status_line = _http_version + blank + std::to_string(_code) + blank + _desc + line_break;
        std::string resp_header;
        for (auto &head : _headers)
        {
            std::string line = head.first + linesep + head.second + line_break;
            resp_header += line;
        }
        return status_line + resp_header + line_break + _text;
    }
    // void Deserialize() // 只做服务端，不必要
    // {

    // }
    void SetCode(int code)
    {
        _code = code;
        switch (_code)
        {
        case 200:
            _desc = "OK";
            break;
        case 404:
            _desc = "Not Found";
            break;
        default:
            break;
        }
    }
    bool SetHeader(const std::string &k, const std::string &v)
    {
        auto it = _headers.find(k);
        if (it != _headers.end()) // 不存在这个pair
            return false;

        _headers.insert(std::make_pair(k, v));
        return true;
    }
    std::string Uri2Suffix(const std::string &filename)
    {
        // ./wwwroot/imag/红叶.jpg
        auto pos = filename.rfind(".");
        if (pos == std::string::npos)
            return "text/html";

        std::string suffix = filename.substr(pos); // pos是"."的位置，前闭后开区间 切取到末尾
        if (suffix == ".html" || suffix == ".htm")
            return "text/html";
        else if (suffix == ".jpg")
            return "image/jpeg";
        else if (suffix == ".png")
            return "imag/png";
        else
            return "";
    }
    void MakeResp()
    {
        bool res = Tool::GetFileContent(_target_file, &_text);
        if (!res) // 将文件正文读取到_text里
        {
            // 目标文件不存在，需要重定向到404页面
            SetCode(404);
            SetTargetFile("./wwwroot/404.html");
            // 正文大小
            int filesize = Tool::FileSize(_target_file);
            // 文件类型（后缀）
            std::string suffix = Uri2Suffix(_target_file); // 将Uri转为文件后缀返回

            // std::cout << "_target_file: " << _target_file << std::endl;
            // std::cout << "suffix: " << suffix << std::endl;

            // 搞半天结果是中文不支持的原因，避免用中文

            SetHeader("Content-Type", suffix);
            SetHeader("Content-Length", std::to_string(filesize));
            Tool::GetFileContent(_target_file, &_text); // 读取404页面到_text里
        }
        else
        {
            SetCode(200);
            // 正文大小
            int filesize = Tool::FileSize(_target_file);
            // 文件类型（后缀）
            std::string suffix = Uri2Suffix(_target_file); // 将Uri转为文件后缀返回

            // std::cout << "_target_file: " << _target_file << std::endl;
            // std::cout << "suffix: " << suffix << std::endl;

            SetHeader("Content-Type", suffix);
            SetHeader("Content-Length", std::to_string(filesize));
            // SetHeader("Set_Cookie", "username=wangqin;"); // 设置cookie
        }
    }
    void SetText(std::string &text)
    {
        _text = text;
    }
    void SetTargetFile(std::string filename)
    {
        _target_file = filename;
    }

private:
    std::string _http_version; // http版本
    int _code;                 // 状态码
    std::string _desc;         // 状态码描述

    std::unordered_map<std::string, std::string> _headers; // 响应报头

    std::string _text; // 响应正文

    std::string _target_file; // 要请求访问的文件
};

class Http
{
public:
    Http() {}
    ~Http() {}
    void HanderRequest(std::shared_ptr<Socket> &sock, InetAddr &addr)
    {
        std::string recv_str;
        int n = sock->Recv(&recv_str); // 获取客户端请求
        // 假设就是读到了一个完整报文
        if (n > 0)
        {
            Request req;
            // 将请求序列化为request
            req.Deserialize(recv_str);
            LOG(LogLevel::INFO) << "client: " << addr.StringAddr();
            // 构建res
            Response resp;
            // 分析request，主要是看请求哪个网页
            resp.SetTargetFile(req.Uri());
            // 将文件写入正文
            resp.MakeResp();
            // 将resp序列化
            std::string resp_str = resp.Serialize();

            // 返回response
            sock->Send(resp_str);
        }
    }

private:
};