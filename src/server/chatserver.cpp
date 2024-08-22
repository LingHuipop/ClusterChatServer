#include "chatserver.hpp"
#include "json.hpp"
#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr,const string& nameArg)
            :_server(loop,listenAddr,nameArg)   // 构造函数
            ,_loop(loop)
{
    //注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));

    //注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

    //设置线程数量
    _server.setThreadNum(4); 
}

//启动服务
void ChatServer::start()
{
    _server.start();    
}

//上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
        return;
    }
}

//上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //达到的目的:完全解耦网络模块的代码和业务模块的代码
    //通过js["msgid"] 获取一个业务handler=>conn  js time
}
