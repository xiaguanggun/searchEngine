#include <unistd.h>
#include "CacheManager.h"

// 静态变量
CacheManager * CacheManager::_pInstance = nullptr;

// 初始化
void CacheManager::init(size_t threadNum) {
    auto& configs = Configuration::getConfigMap();
    CacheManager * pcache = getInstance();
    pcache->_threadNum = threadNum;
    vector<LRUCache>& cachesKeyWord = pcache->_cachesKeyWord;
    vector<LRUCache>& cachesWebQuery = pcache->_cachesWebQuery;
    const string& cachesKeyWordFileName = configs["cachesKeyWord"];
    const string& cachesWebQueryFileName = configs["cachesWebQuery"];
    LRUCache temp1(cachesKeyWordFileName);
    /* temp1.prints(); */
    LRUCache temp2(cachesWebQueryFileName);
    /* temp2.prints(); */
    cachesKeyWord.assign(threadNum * 2 + 1,temp1);
    cachesWebQuery.assign(threadNum * 2 + 1,temp2);
}

// 同步更新所有缓存
void CacheManager::updateCache() {
    std::cout << "updateCache runing...\n";
    CacheManager * pcache = getInstance();
    size_t threadNum = pcache->_threadNum;
    auto & cachesKeyWord = pcache->_cachesKeyWord;
    auto & cachesWebQuery = pcache->_cachesWebQuery;
    // 收集所有子cache的pendingNodes,加锁
    {
        unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
        for(size_t i = 1; i <= threadNum; ++i){
            cachesKeyWord[i + threadNum]._pendingNodes = cachesKeyWord[i]._pendingNodes;
            cachesWebQuery[i + threadNum]._pendingNodes = cachesWebQuery[i]._pendingNodes;
        }
    }
    sleep(10);
    // 同步到主cache
    for(size_t i = 1; i <= threadNum; ++i){
        cachesKeyWord[0].update(cachesKeyWord[i + threadNum]);
        cachesWebQuery[0].update(cachesWebQuery[i + threadNum]);
    }
    // 拷贝回所有子线程的备份cache
    for(size_t i = 1; i <= threadNum; ++i){
        cachesKeyWord[i + threadNum] = cachesKeyWord[0];
        cachesWebQuery[i + threadNum] = cachesWebQuery[0];
    }
    // 交换所有子线程的cache和备份cache,加锁
    {
        unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
        for(size_t i = 1; i <= threadNum; ++i){
            std::swap(cachesKeyWord[i],cachesKeyWord[i + threadNum]);
            std::swap(cachesWebQuery[i],cachesWebQuery[i + threadNum]);
        }
    }
    std::cout << "updateCache finished...\n";
}

// 缓存持久化
void CacheManager::writeToFile(){
    CacheManager * self = getInstance();
    auto& configs = Configuration::getConfigMap();
    const string& cachesKeyWordFileName = configs["cachesKeyWord"];
    const string& cachesWebQueryFileName = configs["cachesWebQuery"];
    self->_cachesKeyWord[0].writeToFile(cachesKeyWordFileName);
    self->_cachesWebQuery[0].writeToFile(cachesWebQueryFileName);
}
