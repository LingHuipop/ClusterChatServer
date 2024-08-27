#ifndef FRIENDMODEL_HPP
#define FRIENDMODEL_HPP
#include "user.hpp"
#include <vector>
//维护好友信息的操作接口方法
class FriendModel 
{
public:
    //添加好友
    void insert(int userid,int friendid);
    //返回用户的好友列表 
    vector<User> query(int userid);

};
#endif