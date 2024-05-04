#ifndef _TASK_H
#define _TASK_H

#include "TcpConnection.h"

class Task{
public:
    Task(const string& msg, const TcpConnectionPtr con)
    :_msg(msg)
    ,_con(con)
    {}
    virtual void taskFunc() const {
        // 普通任务
        _con->sendInLoop(_msg);
    }
protected:
    string _msg;
    TcpConnectionPtr _con;
};

class WebQueryTask:public Task
{
public:
    WebQueryTask(const string& msg, const TcpConnectionPtr con)
    :Task(msg,con)
    {}
    void taskFunc() const override{
        // 网页查询
        _con->sendInLoop(_msg);
    }
};

class KeyWordTask:public Task
{
public:
    KeyWordTask(const string& msg, const TcpConnectionPtr con)
    :Task(msg,con)
    {}
    void taskFunc() const override{
        // 关键字推送
        _con->sendInLoop(_msg);
    }
};

#endif
