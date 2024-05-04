/**
 * Project Reactor
 */


#include "TcpServer.h"

/**
 * TcpServer implementation
 */


/**
 * @param ip
 * @param port
 * @param timeout
 */
TcpServer::TcpServer(const string& ip,unsigned short port,int timeout)
:_acceptor(ip,port)
,_loop(_acceptor,timeout)
{}

/**
 * @param ip
 * @param port
 * @param timeout
 */
TcpServer::TcpServer(const string& ip,const string& port,int timeout)
:_acceptor(ip,port)
,_loop(_acceptor,timeout)
{}

/**
 * @param cb1
 * @param cb2
 * @param cb3
 */
void TcpServer::setAllCallback(Callback && cb1, Callback && cb2, Callback && cb3) {
    _loop.saveNewConnCb(std::move(cb1));
    _loop.saveMessageCb(std::move(cb2));
    _loop.saveCloseCb(std::move(cb3));
}

void TcpServer::start() {
    _acceptor.ready();
    _loop.loop();
}

void TcpServer::stop() {
    _loop.unloop();
}
