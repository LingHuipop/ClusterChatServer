#include "chatservice.hpp"
#include "public.hpp"
#include <string>
#include <vector>
#include <muduo/base/Logging.h>

using namespace std;
using namespace muduo;

// 获取单例对象的接口函数
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}


//注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)}); 
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)}); 
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
}

 //服务器异常，业务重置方法
 void ChatService::reset()
 {
    //把所有用户状态置为offline
    _userModel.resetState();
 }

//处理登录业务 id name password
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    //string name = js["name"];
    string pwd = js["password"];

    User user = _userModel.query(id);

    if(user.getId() == id && user.getPwd()==pwd)
    {
        if(user.getState() == "online")
        {
            //该用户已经上线，不允许重复登录
            json response;
            response["msgId"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该用户已经上线,请勿重复登录";
            conn->send(response.dump()); 
        }
        else
        {
            //登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id,conn});
            }

            //登录成功，更新用户状态为online
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgId"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            //查询该用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if(!vec.empty())
            {
                response["offlineMsg"] = vec;
                //读取离线消息后，将该用户离线消息置空
                _offlineMsgModel.remove(id);
            }
            conn->send(response.dump());

        }

    }
    else
    {
        //该用户不存在 登录失败 或者密码错误
        json response;
        response["msgId"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "该用户不存在或者密码错误";
        conn->send(response.dump());        
    }
}

//处理注册业务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);;
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if(state)
    {
        //注册成功，返回登录成功
        json response;
        response["msgId"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        //注册失败，返回登录失败
        json response;
        response["msgId"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

MsgHandler ChatService::getHandler(int msgId)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgId);
    if(it == _msgHandlerMap.end())
    {
        //返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time){
            LOG_ERROR << "msgId:"<< msgId <<"can not found !!!";
        };

    }
    else
    {
        return _msgHandlerMap[msgId];
    }
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    //更新用户状态为离线
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }

}
//一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end())
        {
            //toid在线,转发消息 服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    //toid不在线，记录离线消息
    _offlineMsgModel.insert(toid, js.dump());
}