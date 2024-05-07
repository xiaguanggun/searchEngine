#ifndef _TASK_H
#define _TASK_H

#include "TcpConnection.h"
#include "../online/Dictionary.h"
#include "../online/WebPageQuery.h"
#include "../cache/CacheManager.h"

extern thread_local size_t _threadId;

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

class KeyWordTask:public Task
{
public:
    KeyWordTask(const string& msg, const TcpConnectionPtr con)
    :Task(msg,con)
    {}
    void taskFunc() const override{
        // 关键字推送,先查缓存,调用Dictionary类的doQuery函数
        /* std::cout << _threadId << "\n"; */
        /* LRUCache & cache = CacheManager::getKeyWordCache(_threadId); */
        string msg = CacheManager::queryKeyWordCache(_threadId,_msg);
        if(msg.size() == 0){
            msg = Dictionary::doQuery(_msg);
            CacheManager::addKeyWordCache(_threadId,_msg,msg);
        }
        _con->sendInLoop(msg);
    }
};

class WebQueryTask:public Task
{
public:
    WebQueryTask(const string& msg, const TcpConnectionPtr con)
    :Task(msg,con)
    {}
    void taskFunc() const override{
        // 网页查询,先查缓存,再调用WebPageQuery类的doQuery函数
        /* std::cout << _threadId << "\n"; */
        /* LRUCache & cache = CacheManager::getWebQueryCache(_threadId); */
        string msg = CacheManager::queryWebQueryCache(_threadId,_msg);
        if(msg.size() == 0){
            msg = WebPageQuery::doQuery(_msg);
            CacheManager::addWebQueryCache(_threadId,_msg,msg);
        }
        _con->sendInLoop(msg);
    }
};

#endif
