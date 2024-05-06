#ifndef _CACHEMANAGER_H
#define _CACHEMANAGER_H

#include <vector>
#include "LRUCache.h"
#include "../shared/Configuration.h"
using std::vector;

class CacheManager {
public: 
    static CacheManager * getInstance();
    static void init(const string& filename, size_t threadNum);
    static LRUCache& getCache(size_t idx);
    static void updateCache();
private:
    CacheManager();
    ~CacheManager(){}
    static void destory();
private: 
    static CacheManager * _pInstance;
    vector<LRUCache> _caches;
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
}

// 获取Cache
inline LRUCache& CacheManager::getCache(size_t idx){
    return getInstance()->_caches[idx];
}

#endif //_CACHEMANAGER_H
