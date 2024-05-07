#ifndef _TIMERMANAGER_H
#define _TIMERMANAGER_H

#include <unistd.h>
#include <sys/timerfd.h>
#include <thread>
#include <memory>
#include <vector>
#include "TimerTask.hpp"
#include "../reactor/TaskQueue.h"
using std::thread;
using std::unique_ptr;
using std::shared_ptr;
using std::vector;

class TimerTask;
class TimerManager {
public: 
    TimerManager(size_t qSize = 1,int initSec = 10, int peridocSec = 30);
    ~TimerManager(){ ::close(_timerfd); } // 关闭timerfd
    void start(); // 创建 & 启动_timerfd
    void stop(); // 停止
    void addTask(); // 添加任务到list
    void doTask(); // 执行TimerTask
    int fd(){ return _timerfd; };
private:
    int createTimerFd();
    void setTimer(int initSec, int peridocSec);
private: 
    int _timerfd;
    bool _isExit; // 是否退出
    int _initSec; // 起始时间
    int _peridocSec; // 周期时间(闹钟的间隔时间)
    unique_ptr<thread> _pthread; // 自己的子线程
    /* vector<Task *> _taskQueue; // timer任务队列,从后面pop并执行 */
    TaskQueue<shared_ptr<TimerTask>> _taskQ; // timer自己的任务队列
};

inline int TimerManager::createTimerFd(){
    int fd = ::timerfd_create(CLOCK_REALTIME, 0);
    if(fd < 0){
        LogError("::timerfd_create");
    }
    return fd;
}

#endif //_TIMERMANAGER_H
