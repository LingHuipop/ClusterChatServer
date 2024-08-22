#include "chatserver.hpp"
#include <iostream>

using namespace std;
int main()
{
    EventLoop loop;
    InetAddress Addr("127.0.0.1", 6000);

    ChatServer server(&loop, Addr,"ChatServer");
    server.start();
    loop.loop();

    return 0;
}