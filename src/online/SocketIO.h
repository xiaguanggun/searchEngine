/**
 * Project Reactor
 */


#ifndef _SOCKETIO_H
#define _SOCKETIO_H

class SocketIO {
public: 
    
/**
 * @param fd
 */
SocketIO(int fd);
    
/**
 * @param buf
 * @param len
 */
int readn(char * buf, int len);
    
/**
 * @param buf
 * @param len
 */
int writen(const char * buf, int len);
    
/**
 * @param buf
 * @param len
 */
int readline(char * buf, int len);
private: 
    int _fd;
};

#endif //_SOCKETIO_H
