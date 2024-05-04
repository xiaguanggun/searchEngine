/**
 * Project Reactor
 */


#ifndef _REACTOR_H
#define _REACTOR_H

#include "TcpServer.h"
#include "ThreadPool.h"

class Reactor {
public: 
    
/**
 * @param ip
 * @param port
 * @param timeout
 * @param threadNum
 * @param qSize
 */
Reactor(size_t threadNum, size_t qSize, const string& ip, unsigned short port, int timeout = 3000);
    
/**
 * @param ip
 * @param port
 * @param timeout
 * @param threadNum
 * @param qSize
 */
Reactor(size_t threadNum, size_t qSize, const string& ip, const string& port, int timeout = 3000);
    
~Reactor();
    
void start();
    
void stop();
    
/**
 * @param con
 */
void onNewConn(TcpConnectionPtr con);
    
/**
 * @param con
 */
void onMessage(TcpConnectionPtr con);
    
/**
 * @param con
 */
void onClose(TcpConnectionPtr con);
private: 
    ThreadPool _pool;
    TcpServer _server;
};

#endif //_REACTOR_H
