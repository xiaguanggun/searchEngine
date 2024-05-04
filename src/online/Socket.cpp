/**
 * Project Reactor
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "Socket.h"
#include "macro.h"

/**
 * Socket implementation
 */

Socket::Socket(){
    _fd = ::socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(_fd,-1,"::socket");
}

/**
 * @param fd
 */
Socket::Socket(int fd)
:_fd(fd)
{}

Socket::~Socket() {
    close(_fd);
}

/**
 * @return int
 */
int Socket::fd() {
    return _fd;
}
