#include "ThreadPool.h"
#include "Task.h"
#include <unistd.h>

ThreadPool::ThreadPool(size_t threadNum, size_t qSize)
:_threadNum(threadNum)
,_qSize(qSize)
,_taskQ(qSize)
,_isExit(false)
{
    _threads.reserve(threadNum);
}

void ThreadPool::start() {
    for(size_t i = 0; i < _threadNum; ++i){
        _threads.emplace_back(new thread(&ThreadPool::doTask,this));
    }
}

void ThreadPool::stop() {
    // 保证任务执行完毕
    while(!_taskQ.empty()){
        sleep(1);
    }
    // 修改标志位
    _isExit = true;
    // 唤醒所有线程&回收
    _taskQ.notify_all();
    for(auto &th : _threads){
        th->join();
    }
}

void ThreadPool::addTask(shared_ptr<Task> ptask) {
    if(ptask){
        _taskQ.push(ptask);
    }
}

void ThreadPool::doTask() {
    while(!_isExit){
        auto ptask = getTask();
        if(ptask){
            ptask->taskFunc();
        }
    }
}

shared_ptr<Task> ThreadPool::getTask() {
    return _taskQ.pop();
}
