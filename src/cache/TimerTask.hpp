#ifndef _TIMERTASK_H
#define _TIMERTASK_H

#include "CacheManager.h"

class TimerTask
{
public:
    void taskFunc() const{
        // 更新缓存系统,调用CacheManager的updateCache函数
        CacheManager::updateCache();
    }
};

#endif
