#include <unistd.h>
#include "CacheManager.h"
#include "TimerManager.h"

extern thread_local size_t _threadId;

TimerManager::TimerManager(size_t qSize,int initSec, int peridocSec)
:_timerfd(createTimerFd())
,_isExit(false)
,_initSec(initSec)
,_peridocSec(peridocSec)
,_taskQ(qSize)
{}

void TimerManager::start() {
    setTimer(_initSec,_peridocSec);
    _pthread.reset(new thread(&TimerManager::doTask,this)); // 创建子线程执行
}

void TimerManager::stop() {
    // 停止_timerfd
    setTimer(0,0);
    // 保证任务执行完毕
    while(!_taskQ.empty()){
        sleep(1);
    }
    // 修改标志位
    _isExit = true;
    // 唤醒所有线程&回收
    _taskQ.notify_all();
    _pthread->join();
}

void TimerManager::addTask() {
    // read timerfd,防止timerfd一直就绪
    uint64_t two;
    ssize_t ret = read(_timerfd, &two, sizeof(uint64_t));
    if(ret != sizeof(uint64_t)){
        LogError("read timerfd failed");
        return;
    }
    // 添加TimerTask
    /* cout << "addTask\n"; */
    _taskQ.push(shared_ptr<TimerTask>(new TimerTask));
}

void TimerManager::doTask() {
    /* cout << _threadId << "\n"; */
    while(!_isExit){
        auto ptask = _taskQ.pop();
        /* cout << "do TimerTask\n"; */
        if(ptask){
            ptask->taskFunc();                                                                                                                         
        }
    }
}

void TimerManager::setTimer(int initSec, int peridocSec){
    struct itimerspec value;
    value.it_value.tv_sec = initSec;
    value.it_value.tv_nsec = 0;

    value.it_interval.tv_sec = peridocSec;
    value.it_interval.tv_nsec = 0;

    int ret = ::timerfd_settime(_timerfd, 0, &value, nullptr);
    if(ret){
        LogError("::timerfd_settime");
    }
}

