/**
 * Project Reactor
 */


#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoop.h"

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using Callback = function<void(const TcpConnectionPtr&)>;

class TcpServer {
public: 
    
/**
 * @param ip
 * @param port
 * @param timeout
 */
TcpServer(const string& ip, unsigned short port, int timeout = 3000);
    
/**
 * @param ip
 * @param port
 * @param timeout
 */
TcpServer(const string& ip, const string& port, int timeout = 3000);
    
/**
 * @param cb1
 * @param cb2
 * @param cb3
 */
void setAllCallback(Callback && cb1, Callback && cb2, Callback && cb3);
    
void start();
    
void stop();
private: 
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif //_TCPSERVER_H
