/**
 * Project Reactor
 */


#include <sys/epoll.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <iostream>

#include "EventLoop.h"
#include "macro.h"
#include "Acceptor.h"
#include "TcpConnection.h"

/**
 * EventLoop implementation
 */


/**
 * @param acceptor
 * @param timeout
 */
EventLoop::EventLoop(Acceptor& acceptor,int timeout)
:_epfd(createEpollFd())
,_timeOut(timeout)
,_readySet(1)
,_isLooping(false)
,_acceptor(acceptor)
,_eventfd(createEventFd())
,_timerManager()
,_mutex(mutex())
{
    // sockfd加入监听
    addEpollReadFd(_acceptor.fd());
    // stdin加入监听
    addEpollReadFd(STDIN_FILENO);
    // eventfd加入监听
    addEpollReadFd(_eventfd.fd());
    // timerfd加入监听
    addEpollReadFd(_timerManager.fd());
}

EventLoop::~EventLoop() {
    ::close(_epfd);
}

/**
 * @return void
 */
void EventLoop::loop() {
    _isLooping = true;
    _timerManager.start();
    while(_isLooping){
        waitEpollFd();
    }
}

/**
 * @return void
 */
void EventLoop::unloop() {
    _isLooping = false;
    _timerManager.stop();
}

/**
 * @param cb
 * @return void
 */
void EventLoop::saveNewConnCb(TcpConnectionCb && cb) {
    _onNewConnCb = std::move(cb);
}

/**
 * @param cb
 * @return void
 */
void EventLoop::saveMessageCb(TcpConnectionCb && cb) {
    _onMessageCb = std::move(cb);
}

/**
 * @param cb
 * @return void
 */
void EventLoop::saveCloseCb(TcpConnectionCb && cb) {
    _onCloseCb = std::move(cb);
}

/**
 * @param cb
 * @return void
 */
void EventLoop::runInLoop(Functor && cb) {
    // 上锁 & 放入任务
    {
        std::unique_lock<mutex> autolock(_mutex);
        _pendings.push_back(std::move(cb));
    }
    // 唤醒EventLoop
    wakeup();
}

/**
 * @return int
 */
int EventLoop::createEpollFd() {
    int epfd = ::epoll_create(1);
    ERROR_CHECK(epfd,-1,"::epoll_create");
    return epfd;
}

/**
 * @param fd
 * @return void
 */
void EventLoop::addEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;
    int ret = ::epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&evt);
    ERROR_CHECK(ret,-1,"::epoll_ctl add");
}

/**
 * @param fd
 * @return void
 */
void EventLoop::delEpollReadFd(int fd) {
    int ret = ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,nullptr);
    ERROR_CHECK(ret,-1,"::epoll_ctl del");
}

/**
 * @return void
 */
void EventLoop::waitEpollFd() {
    int readyNum = ::epoll_wait(_epfd,_readySet.data(),_readySet.capacity(),_timeOut);
    ERROR_CHECK(readyNum,-1,"::epoll_wait");
    // 是否timeout
    if(readyNum == 0){
        std::cout << ">>epoll_wait timeout!\n";
        return;
    }
    // 是否需要扩容
    if(readyNum == static_cast<int>(_readySet.capacity())){
        size_t newCap = _readySet.capacity() << 1;
        _readySet.resize(newCap);
        std::cout << ">>reserve readySet, newCapacity = " << newCap << "\n";
    }
    // 处理
    for(int i = 0; i < readyNum; ++i){
        int fd = _readySet[i].data.fd;
        if(fd == _acceptor.fd()){
            handleNewConn();
        }
        else if(fd == STDIN_FILENO){
            std::string cmd;
            std::cin >> cmd;
            if(cmd == "quit"){
                std::cout << ">>cmd = quit\n";
                unloop();
            }
            else{
                std::cout << ">>invalid cmd!\n";
            }
        }
        else if(fd == _eventfd.fd()){
            handleRead();
            doPendingFunctors();
        }
        else if(fd == _timerManager.fd()){
            _timerManager.addTask();
        }
        else{
            handleMessage(fd);
        }
    }
}

/**
 * @return void
 */
void EventLoop::handleNewConn() {
    // accept
    int fd = _acceptor.accept();
    addEpollReadFd(fd);
    // 创建Tcp连接
    TcpConnectionPtr pTcp(new TcpConnection(fd,this));
    _conns[fd] = pTcp;
    // 设置&执行cb函数
    pTcp->setNewConnCb(_onNewConnCb);
    pTcp->setMessageCb(_onMessageCb);
    pTcp->setCloseCb(_onCloseCb);
    pTcp->handleNewConnCb();
}

/**
 * @param fd
 * @return void
 */
void EventLoop::handleMessage(int fd) {
    TcpConnectionPtr pTcp = _conns[fd];
    // 对端是否关闭
    if(pTcp->isClosed()){
        pTcp->handleCloseCb();
        delEpollReadFd(fd);
        _conns.erase(fd);
        return;
    }
    // 未关闭
    pTcp->handleMessageCb();
}

/**
 * @return int
 */
int EventLoop::createEventFd() {
    // 计数器文件描述符初始值大小+flag(已弃用,填0)
    int epfd = ::eventfd(0,0);
    ERROR_CHECK(epfd,-1,"::epoll_create");
    return epfd;
}

/**
 * @return void
 */
void EventLoop::handleRead() {
    // 清除缓冲区
    uint64_t buf;
    int ret = ::read(_eventfd.fd(),&buf,sizeof(buf));
    ERROR_CHECK(ret,-1,"::read");
}

/**
 * @return void
 */
void EventLoop::doPendingFunctors() {
    vector<Functor> temp;
    // 上锁 & 取出任务
    {
        std::unique_lock<mutex> autolock(_mutex);
        temp.swap(_pendings);
    }
    // 执行任务
    for(auto &cb:temp){
        cb();
    }
}

/**
 * @return void
 */
void EventLoop::wakeup() {
    uint64_t buf = 1;
    int ret = ::write(_eventfd.fd(),&buf,sizeof(buf));
    ERROR_CHECK(ret,-1,"::write");
}
