#include "Http.hpp"
#include "Reactor.hpp"
#include "Listener.hpp"
#include "Socket.hpp"
#include "Common.hpp"
#include "daemon.hpp"
#include <fstream>

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
    uint16_t port = std::stoi(argv[1]);

    FileLogStrategy();

    std::unique_ptr<Http> http = std::make_unique<Http>();

    std::shared_ptr<Connection> Con = std::make_shared<Listener>(port);

    Con->SetHander([&http](std::string &inbuffer) -> std::string
                   { return http->HanderRequest(inbuffer); }); // TODO

    std::unique_ptr<Reactor> R = std::make_unique<Reactor>();

    R->AddConnection(Con);

    R->Loop();

    return 0;
}