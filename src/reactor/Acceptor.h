/**
 * Project Reactor
 */


#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "Socket.h"
#include "InetAddress.h"

class Acceptor {
public: 
    
/**
 * @param ip
 * @param port
 */
Acceptor(const string& ip, unsigned short port);
    
/**
 * @param ip
 * @param port
 */
Acceptor(const string& ip, const string& port);
    
void ready();
    
int accept();
    
int fd();
private: 
    Socket _sock;
    InetAddress _addr;
    
void setReuseAddr();
    
void setReusePort();
    
void bind();
    
void listen();
};

#endif //_ACCEPTOR_H
