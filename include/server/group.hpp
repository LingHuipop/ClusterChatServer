#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

// User表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setDesc(string desc) { this->desc = desc; }

    int getId() const { return this->id; }
    string getName() const { return this->name; }
    string getDesc() const { return this->desc; }
    vector<GroupUser> &getUsers() const { return this->users; }

private:
    int id;//组id
    string name;//组名称
    string desc;//组功能描述
    vector<GroupUser> users;//给业务层使用
};

#endif