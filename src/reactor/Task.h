#ifndef _TASK_H
#define _TASK_H

#include "TcpConnection.h"
#include "../online/Dictionary.h"
#include "../online/WebPageQuery.h"

class Task{
public:
    Task(const string& msg, const TcpConnectionPtr con)
    :_msg(msg)
    ,_con(con)
    {}
    virtual void taskFunc() const {
        // 普通任务,回复客户端信息
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
        // 网页查询,调用WebPageQuery类的doQuery函数
        const string& msg = WebPageQuery::doQuery(_msg);
        _con->sendInLoop(msg);
    }
};

class KeyWordTask:public Task
{
public:
    KeyWordTask(const string& msg, const TcpConnectionPtr con)
    :Task(msg,con)
    {}
    void taskFunc() const override{
        // 关键字推送,调用Dictionary类的doQuery函数
        const string& msg = Dictionary::doQuery(_msg);
        _con->sendInLoop(msg);
    }
};

#endif
