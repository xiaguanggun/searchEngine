/**
 * Project Reactor
 */


#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

#include "EventFd.h"
using std::vector;
using std::map;
using std::shared_ptr;
using std::function;
using std::mutex;

class TcpConnection;
class Acceptor;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCb = function<void(const TcpConnectionPtr&)>;
using Functor = function<void()>;

class EventLoop {
public: 
    
/**
 * @param acceptor
 * @param timeout
 */
EventLoop(Acceptor& acceptor, int timeout = 3000);
    
~EventLoop();
    
void loop();
    
void unloop();
    
/**
 * @param cb
 */
void saveNewConnCb(TcpConnectionCb && cb);
    
/**
 * @param cb
 */
void saveMessageCb(TcpConnectionCb && cb);
    
/**
 * @param cb
 */
void saveCloseCb(TcpConnectionCb && cb);
    

/**
 * @param cb
 */
void runInLoop(Functor && cb);
private: 
    int _epfd;
    int _timeOut;
    vector<struct epoll_event> _readySet;
    bool _isLooping;
    Acceptor& _acceptor;
    map<int,shared_ptr<TcpConnection>> _conns;
    TcpConnectionCb _onNewConnCb;
    TcpConnectionCb _onMessageCb;
    TcpConnectionCb _onCloseCb;
    EventFd _eventfd;
    mutex _mutex;
    vector<Functor> _pendings;
    
int createEpollFd();
    
/**
 * @param fd
 */
void addEpollReadFd(int fd);
    
/**
 * @param fd
 */
void delEpollReadFd(int fd);
    
void waitEpollFd();
    
void handleNewConn();
    
/**
 * @param fd
 */
void handleMessage(int fd);
    
int createEventFd();
    
void handleRead();
    
void doPendingFunctors();
    
void wakeup();
};

#endif //_EVENTLOOP_H
