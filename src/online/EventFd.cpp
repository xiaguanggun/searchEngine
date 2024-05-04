/**
 * Project Reactor
 */


#include <sys/eventfd.h>
#include <unistd.h>

#include "EventFd.h"
#include "macro.h"

/**
 * Eventfd implementation
 */

EventFd::EventFd(){
    // 两个参数,第一个为初始值,第二个为flag = 0
    _fd = ::eventfd(0,0);
    ERROR_CHECK(_fd,-1,"::evevtfd");
}

/**
 * @param fd
 */
EventFd::EventFd(int fd)
:_fd(fd)
{}

EventFd::~EventFd() {
    close(_fd);
}

/**
 * @return int
 */
int EventFd::fd() {
    return _fd;
}
