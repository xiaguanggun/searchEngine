/**
 * Project Reactor
 */


#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include <string>
#include <memory>
#include <functional>

#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"
using std::shared_ptr;
using std::function;

class TcpConnection;
class EventLoop;
using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCb = function<void(const TcpConnectionPtr&)>;

class TcpConnection
:public std::enable_shared_from_this<TcpConnection>
{
public: 
    
/**
 * @param fd
 * @param loop
 */
explicit TcpConnection(int fd, EventLoop* loop);
/* explicit TcpConnection(int fd); */
    
/**
 * @param msg
 */
void send(const string& msg);
    
string recv();

/**
 * @param len
 */
//string recvn(size_t len);

int fd();
    
string toString();
    
bool isClosed();
    
/**
 * @param cb
 */
void setNewConnCb(const TcpConnectionCb& cb);
    
/**
 * @param cb
 */
void setMessageCb(const TcpConnectionCb& cb);
    
/**
 * @param cb
 */
void setCloseCb(const TcpConnectionCb& cb);
    
void handleNewConnCb();
    
void handleMessageCb();
    
void handleCloseCb();
    
/**
 * @param msg
 */
void sendInLoop(const string& msg);
private: 
    EventLoop * _loop;
    SocketIO _sockIO;
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    TcpConnectionCb _onNewConnCb;
    TcpConnectionCb _onMessageCb;
    TcpConnectionCb _onCloseCb;
    
InetAddress getLocalAddr();
    
InetAddress getPeerAddr();
};

#endif //_TCPCONNECTION_H
