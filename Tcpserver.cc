#include "Tcpserver.hpp"
#include "Socket.hpp"
#include "Common.hpp"
#include "Http.hpp"
#include "daemon.hpp"
#include <fstream>
// #include "NetCal.hpp"
// #include "daemon.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port " << std::endl;
}
void echo(std::shared_ptr<Socket> &sock, InetAddr &addr)
{
    std::string mesg;
    sock->Recv(&mesg);
    std::cout << addr.StringAddr() << " : ";
    std::cout << mesg << std::endl;
}
// ./server port
int main(int argc, char *argv[])
{
    daemon(); // 守护进程化

    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t server_port = std::stoi(argv[1]);

    FileLogStrategy();

    std::unique_ptr<Http> http = std::make_unique<Http>();

    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>(server_port, [&http](std::shared_ptr<Socket> &sock, InetAddr &addr)
                                                                    { http->HanderRequest(sock, addr); });

    server->Start();

    return 0;
}