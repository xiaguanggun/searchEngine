#ifndef _TASKQUEUE_H
#define _TASKQUEUE_H

#include <iostream>
#include <queue>
#include "mutex"
#include "condition_variable"
using std::queue;
using std::mutex;
using std::condition_variable;

template<typename T>
class TaskQueue {
public: 
    TaskQueue(size_t qSize);

    inline bool full();
    inline bool empty();

    void push(const T & value);
    T pop();
    void notify_all();
private: 
    size_t _qSize;
    queue<T> _que;
    mutex _mutex;
    condition_variable _notFull; // Producer
    condition_variable _notEmpty;// Consumer
    bool _isExit;
};
#include "TaskQueue.cpp"

#endif //_TASKQUEUE_H
