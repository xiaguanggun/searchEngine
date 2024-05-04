/**
 * Project Reactor
 */


#ifndef _INETADDRESS_H
#define _INETADDRESS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
using std::string;

class InetAddress {
public: 
    
/**
 * @param ip
 * @param port
 */
InetAddress(const string& ip, unsigned short port);
    
/**
 * @param ip
 * @param port
 */
InetAddress(const string& ip, const string& port);
    
/**
 * @param addr
 */
InetAddress(const struct sockaddr_in& addr);
    
string ip();
    
unsigned short port();
    
struct sockaddr_in * getInetAddressPtr();
private: 
    struct sockaddr_in _addr;
};

#endif //_INETADDRESS_H
