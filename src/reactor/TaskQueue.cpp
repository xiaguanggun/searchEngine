#ifndef _TASKQUEUE_CPP
#define _TASKQUEUE_CPP

#include <iostream>
#include "TaskQueue.h"
using std::cout;
using std::unique_lock;

template<typename T>
TaskQueue<T>::TaskQueue(size_t qSize)
:_qSize(qSize)
,_que()
,_mutex()
,_notFull()
,_notEmpty()
,_isExit(false)
{}

template<typename T>
inline bool TaskQueue<T>::full() {
    return _que.size() == _qSize;
}

template<typename T>
inline bool TaskQueue<T>::empty() {
    return _que.size() == 0;
}

template<typename T>
void TaskQueue<T>::push(const T & value) {
    // 1 自动上锁
    unique_lock<mutex> autolock(_mutex);
    // 2 检查是否满
    // 2.1 已满Producer就wait
    while(full()){
        _notFull.wait(autolock);
    }
    // 2.2 未满就入队
    _que.emplace(value);
    // 并唤醒Consumer
    _notEmpty.notify_one();
    // 3 AutoMutex自动析构解锁
}

template<typename T>
T TaskQueue<T>::pop() {
    // 1 自动上锁
    unique_lock<mutex> autolock(_mutex);
    // 2 检查是否空
    // 2.1 已空Consumer就wait
    while(empty() && !_isExit){
        _notEmpty.wait(autolock);
    }
    // 先检查是否需要退出
    if(_isExit){
        return T();
    }
    // 2.2 未空就出队
    T temp = _que.front();
    _que.pop();
    // 并唤醒Producer
    _notFull.notify_one();
    return temp;
    // 3 AutoMutex自动析构解锁
}

template<typename T>
void TaskQueue<T>::notify_all(){
    // 修改标志位
    _isExit = true;
    // 唤醒所有消费者
    _notEmpty.notify_all();
}

#endif //_TASKQUEUE_CPP
