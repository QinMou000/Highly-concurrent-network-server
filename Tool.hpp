#pragma once
#include "Common.hpp"
#include "md4c-html.h"
#include <string>



class Tool
{
public:
    static void md4c_output_callback(const MD_CHAR* data, MD_SIZE size, void* userdata) {
        std::string* html = static_cast<std::string*>(userdata);
        html->append(data, size);
    }
    static std::string MarkdownToHtml(const std::string& md) {
        std::string html;
        md_html(md.c_str(), md.size(), md4c_output_callback, &html, 0, 0);
        return html;
    }
    static bool GetFileContent(std::string filename, std::string *out)
    {
        // std::ifstream in(filename);
        // if (!in.is_open())
        //     return false;
        // std::string line;
        // while (std::getline(in, line))
        // {
        //     *out += line;
        // }
        // in.close();
        // return true;

        // version 2 二进制读取
        int filesize = FileSize(filename);
        if (filesize < 0)
            return false;
        else
        {
            std::ifstream in(filename);
            if (!in.is_open())
                return false;
            out->resize(filesize);
            in.read((char *)(out->c_str()), filesize);
            in.close();
        }
        return true;
    }
    static bool GetOneLine(std::string recv_str, std::string *reqline, std::string line_break)
    {
        auto pos = recv_str.find(line_break); // 找分隔符
        if (pos == std::string::npos)
            return false;
        *reqline = recv_str.substr(0, pos); // 切出需要的段

        recv_str.erase(0, pos + line_break.size()); // 删除第一行和分隔符
        return true;
    }

    static int FileSize(const std::string filename)
    {
        std::ifstream in(filename, std::ios::binary); // 以二进制形式打开文件
        if (!in.is_open())
            return -1;
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg);
        in.close();
        return filesize;
    }

    // void sendhtml(std::shared_ptr<Socket> &sock, InetAddr &addr)
    // {
    //     std::string html = "HTTP/1.1 200 OK\r\n\r\n";
    //     auto res = GetFile("./wwwroot/index.html", &html);
    //     if (res)
    //         sock->Send(html);
    //     else
    //         LOG(LogLevel::FATAL) << "send error";
    // }
};
