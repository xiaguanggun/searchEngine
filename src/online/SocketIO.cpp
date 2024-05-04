/**
 * Project Reactor
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

#include "SocketIO.h"
#include "macro.h"

/**
 * SocketIO implementation
 */


/**
 * @param fd
 */
SocketIO::SocketIO(int fd)
:_fd(fd)
{}

/**
 * @param buf
 * @param len
 * @return int
 */
int SocketIO::readn(char * buf, int len) {
    int total = len;
    while(len > 0){
        int sret = recv(_fd,buf,len,0);
        if(sret == -1 && errno == EINTR){
            continue;
        }
        ERROR_CHECK(sret,-1,"recv on readn");
        if(sret == 0){
            std::cout << "client closed!!!\n";
            break;
        }
        buf += sret;
        len -= sret;
    }
    return total - len;
}

/**
 * @param buf
 * @param len
 * @return int
 */
int SocketIO::writen(const char * buf, int len) {
    int total = len;
    while(len > 0){
        int sret = send(_fd,buf,len,0);
        if(sret == -1 && errno == EINTR){
            continue;
        }
        ERROR_CHECK(sret,-1,"send on writen");
        if(sret == 0){
            break;
        }
        buf += sret;
        len -= sret;
    }
    return total - len;
}

/**
 * @param buf
 * @param len
 * @return int
 */
int SocketIO::readline(char * buf, int len) {
    int total = len;
    while(len > 0){
        // 不清除缓冲区的读取
        int sret = recv(_fd,buf,len,MSG_PEEK);
        if(sret == -1 && errno == EINTR){
            continue;
        }
        ERROR_CHECK(sret,-1,"recv on readn");
        if(sret == 0){
            std::cout << "client closed!!!\n";
            break;
        }
        // 查找是否存在换行
        for(int i = 0; i < sret; ++i){
            if(buf[i] == '\n'){
                sret = recv(_fd,buf,i+1,0);
                return total - len - sret;
            }
        }
        // 没有则清除缓冲区并继续peek
        recv(_fd,buf,sret,0);
        buf += sret;
        len -= sret;
    }
    return total - len;
}
