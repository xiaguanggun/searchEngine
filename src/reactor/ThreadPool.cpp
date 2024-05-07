#include <unistd.h>
#include "ThreadPool.h"
#include "Task.h"

thread_local size_t _threadId = 0;

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
        _threads.emplace_back(new thread(&ThreadPool::doTask,this,i+1));
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

// 子线程入口函数
void ThreadPool::doTask(size_t threadId) {
    // 初始化thread_local
    _threadId = threadId;
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
