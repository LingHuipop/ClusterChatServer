#ifndef PUBLIC_H
#define PUBLIC_H
/*
存储消息id和其对应的业务处理方法
*/
enum EnMsgType{
    LOGIN_MSG = 1,//   登录消息
    LOGIN_MSG_ACK,//   登录消息应答
    REG_MSG,     //   注册消息
    REG_MSG_ACK //   注册消息应答

};

#endif
