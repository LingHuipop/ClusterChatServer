#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

void resetHandler(int sig)
{
    ChatService::instance()->reset();
    exit(0);
}
using namespace std;
int main()
{
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress Addr("127.0.0.1", 6000);

    ChatServer server(&loop, Addr,"ChatServer");
    server.start();
    loop.loop();

    return 0;
}