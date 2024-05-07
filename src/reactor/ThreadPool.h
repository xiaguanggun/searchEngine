#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <vector>
#include <memory>
#include <thread>

#include "TaskQueue.h"

using std::vector;
using std::shared_ptr;
using std::thread;

class Thread;
class Task;

class ThreadPool {
public: 
    ThreadPool(size_t threadNum, size_t qSize);
    void start();
    void stop();
    void addTask(shared_ptr<Task> ptask);
    void doTask();
private:
    shared_ptr<Task> getTask();
private: 
    size_t _threadNum;
    size_t _qSize;
    vector<shared_ptr<thread>> _threads;
    TaskQueue<shared_ptr<Task>> _taskQ;
    bool _isExit;
};

#endif //_THREADPOOL_H
