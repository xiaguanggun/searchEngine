/**
 * Project Reactor
 */


#ifndef _EVENTFD_H
#define _EVENTFD_H

class EventFd {
public: 
    
EventFd();
    
/**
 * @param fd
 */
explicit EventFd(int fd);
    
~EventFd();
    
int fd();
private: 
    int _fd;
};

#endif //_EVENTFD_H
