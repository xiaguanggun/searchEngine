#ifndef _CACHEMANAGER_H
#define _CACHEMANAGER_H

#include <vector>
#include "LRUCache.h"
#include "../shared/Configuration.h"
using std::vector;

class CacheManager {
public: 
    static CacheManager * getInstance();
    static void init(size_t threadNum);
    static LRUCache& getKeyWordCache(size_t idx);
    static LRUCache& getWebQueryCache(size_t idx);
    static void updateCache();
    static void writeToFile();
private:
    CacheManager();
    ~CacheManager(){}
    static void destory();
private: 
    static CacheManager * _pInstance;
    size_t _threadNum;
    vector<LRUCache> _cachesKeyWord; // 关键字缓存
    vector<LRUCache> _cachesWebQuery; // 网页缓存
};

// 构造析构
inline CacheManager * CacheManager::getInstance(){
    if(!_pInstance){
        _pInstance = new CacheManager();
    }
    return _pInstance;
}
inline void CacheManager::destory(){
    if(_pInstance){
        delete _pInstance;
        _pInstance = nullptr;
    }
}
inline CacheManager::CacheManager(){
    ::atexit(&CacheManager::destory); // 自动销毁
    ::atexit(&CacheManager::writeToFile); // 缓存持久化
}

// 获取Cache
inline LRUCache& CacheManager::getKeyWordCache(size_t idx){
    return getInstance()->_cachesKeyWord[idx];
}
inline LRUCache& CacheManager::getWebQueryCache(size_t idx){
    return getInstance()->_cachesWebQuery[idx];
}

#endif //_CACHEMANAGER_H
