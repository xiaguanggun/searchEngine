/**
 * Project Reactor
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <sstream>
#include <iostream>

#include "TcpConnection.h"
#include "EventLoop.h"
#include "macro.h"
using std::cout;

/**
 * TcpConnection implementation
 */


/**
 * @param fd
 * @param loop
 */
TcpConnection::TcpConnection(int fd,EventLoop * loop)
/* TcpConnection::TcpConnection(int fd) */
:_loop(loop)
,_sockIO(fd)
,_sock(fd)
,_localAddr(getLocalAddr())
,_peerAddr(getPeerAddr())
{}

/**
 * @param msg
 * @return void
 */
void TcpConnection::send(const string& msg) {
    string temp = msg + "\n";
    _sockIO.writen(temp.c_str(),temp.size());
}

/**
 * @return string
 */
string TcpConnection::recv() {
    char buf[1024] = {0};
    _sockIO.readline(buf,sizeof(buf));
    return buf;
}

/**
 * @return string
 */
//string TcpConnection::recvn(size_t len) {
//    string temp;
//    while(len > 0){
//        char buf[1025] = {0};
//        if(len > 1024){
//            _sockIO.readn(buf,1024);
//        }
//        else{
//            _sockIO.readn(buf,len);
//        }
//        temp += buf;
//        len -= 1024;
//    }
//    return temp;
//}

int TcpConnection::fd(){
    return _sock.fd();
}

/**
 * @return string
 */
string TcpConnection::toString() {
    std::ostringstream oss;
    oss <<  _localAddr.ip() << ":"
        << _localAddr.port() << "<--->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();
    return oss.str();
}

/**
 * @return bool
 */
bool TcpConnection::isClosed() {
    char buf[10] = {0};
    int ret = ::recv(_sock.fd(),buf,sizeof(buf),MSG_PEEK);
    return ret == 0;
}

/**
 * @param cb
 * @return void
 */
void TcpConnection::setNewConnCb(const TcpConnectionCb& cb) {
    _onNewConnCb = cb;
    return;
}

/**
 * @param cb
 * @return void
 */
void TcpConnection::setMessageCb(const TcpConnectionCb& cb) {
    _onMessageCb = cb;
    return;
}

/**
 * @param cb
 * @return void
 */
void TcpConnection::setCloseCb(const TcpConnectionCb& cb) {
    _onCloseCb = cb;
    return;
}

/**
 * @return void
 */
void TcpConnection::handleNewConnCb() {
    if(_onNewConnCb){
        _onNewConnCb(shared_from_this());
    }
    return;
}

/**
 * @return void
 */
void TcpConnection::handleMessageCb() {
    if(_onMessageCb){
        _onMessageCb(shared_from_this());
    }
    return;
}

/**
 * @return void
 */
void TcpConnection::handleCloseCb() {
    if(_onCloseCb){
        _onCloseCb(shared_from_this());
    }
    return;
}

/**
 * @param msg
 * @return void
 */
void TcpConnection::sendInLoop(const string& msg) {
    if(_loop){
        _loop->runInLoop(std::bind(&TcpConnection::send,this,msg));
    }
}

/**
 * @return InetAddress
 */
InetAddress TcpConnection::getLocalAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    //获取本端地址的函数getsockname
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
    ERROR_CHECK(ret,-1,"getsockname");
    return addr;
}

/**
 * @return InetAddress
 */
InetAddress TcpConnection::getPeerAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    //获取本端地址的函数getpeername
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
    ERROR_CHECK(ret,-1,"getpeername");
    return addr;
}
