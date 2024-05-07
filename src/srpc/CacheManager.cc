#include <unistd.h>
#include <iostream>
#include "CacheManager.h"

// 静态变量
CacheManager * CacheManager::_pInstance = nullptr;

// 初始化
void CacheManager::init(size_t cacheNum, const string& cacheFileName) {
    CacheManager * pcache = getInstance();
    pcache->_cacheNum = cacheNum;
    pcache->_cacheFileName = cacheFileName;
    vector<Cache>& caches = pcache->_caches;
    Cache temp(cacheFileName);
    caches.assign(cacheNum * 2 + 1,temp);
}

// 同步更新所有缓存
void CacheManager::updateCache() {
    std::cout << "updateCache runing...\n";
    CacheManager * pcache = getInstance();
    size_t cacheNum = pcache->_cacheNum;
    auto & caches = pcache->_caches;
    // 收集所有子cache的pendingNodes,加锁
    {
        unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
        for(size_t i = 1; i <= cacheNum; ++i){
            caches[i + cacheNum]._pendingNodes = caches[i]._pendingNodes;
        }
    }
    sleep(10);
    // 同步到主cache
    for(size_t i = 1; i <= cacheNum; ++i){
        caches[0].update(caches[i + cacheNum]);
    }
    // 拷贝回所有子线程的备份cache
    for(size_t i = 1; i <= cacheNum; ++i){
        caches[i + cacheNum] = caches[0];
    }
    // 交换所有子线程的cache和备份cache,加锁
    {
        unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
        for(size_t i = 1; i <= cacheNum; ++i){
            std::swap(caches[i],caches[i + cacheNum]);
        }
    }
    std::cout << "updateCache finished...\n";
}

// 缓存持久化
void CacheManager::writeToFile(){
    CacheManager * self = getInstance();
    self->_caches[0].writeToFile(self->_cacheFileName);
}
