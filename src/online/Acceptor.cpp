/**
 * Project Reactor
 */


#include <sys/types.h>
#include <sys/socket.h>

#include "Acceptor.h"
#include "macro.h"

/**
 * Acceptor implementation
 */


/**
 * @param ip
 * @param port
 */
Acceptor::Acceptor(const string& ip, unsigned short port)
:_sock()
,_addr(ip,port)
{}

/**
 * @param ip
 * @param port
 */
Acceptor::Acceptor(const string& ip, const string& port)
:_sock()
,_addr(ip,port)
{}

/**
 * @return void
 */
void Acceptor::ready() {
    setReuseAddr();
    setReusePort();
    this->bind();
    this->listen();
}

/**
 * @return int
 */
int Acceptor::fd() {
    return _sock.fd();
}

/**
 * @return int
 */
int Acceptor::accept() {
    int connfd = ::accept(_sock.fd(),nullptr,0);
    ERROR_CHECK(connfd,-1,"::accept");
    return connfd;
}

/**
 * @return void
 */
void Acceptor::setReuseAddr() {
    int on = 1;
    int ret = ::setsockopt(_sock.fd(),SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret){
        perror("::setsockopt");
    }
}

/**
 * @return void
 */
void Acceptor::setReusePort() {
    int on = 1;
    int ret = ::setsockopt(_sock.fd(),SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
    if(ret){
        perror("::setsockopt");
    }
}

/**
 * @return void
 */
void Acceptor::bind() {
    int ret = ::bind(_sock.fd(),(struct sockaddr*)_addr.getInetAddressPtr(),sizeof(struct sockaddr_in));
    if(ret){
        perror("::bind");
    }
}

/**
 * @return void
 */
void Acceptor::listen() {
    int ret = ::listen(_sock.fd(),128);
    if(ret){
        perror("::listen");
    }
}
