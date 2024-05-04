/**
 * Project Reactor
 */

#include <string.h>

#include "InetAddress.h"

/**
 * InetAddress implementation
 */


/**
 * @param ip
 * @param port
 */
InetAddress::InetAddress(const string& ip, unsigned short port) {
    ::memset(&_addr,0,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(port);
}

/**
 * @param ip
 * @param port
 */
InetAddress::InetAddress(const string& ip, const string& port){
    ::memset(&_addr,0,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(stoul(port));
}

/**
 * @param addr
 */
InetAddress::InetAddress(const struct sockaddr_in& addr)
:_addr(addr)
{}

/**
 * @return string
 */
string InetAddress::ip() {
    return inet_ntoa(_addr.sin_addr);
}

/**
 * @return unsigned short
 */
unsigned short InetAddress::port() {
    return ntohs(_addr.sin_port);
}

/**
 * @return struct sockaddr_in *
 */
struct sockaddr_in * InetAddress::getInetAddressPtr() {
    return &_addr;
}
