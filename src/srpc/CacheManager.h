#ifndef _CACHEMANAGER_H
#define _CACHEMANAGER_H

#include <vector>
#include <mutex>
#include "Cache.h"
using std::vector;
using std::mutex;
using std::unique_lock;

class CacheManager {
public: 
    static CacheManager * getInstance();
    static void init(size_t cacheNum, const string& cacheFileName);
    // 查询缓存
    static string queryCache(size_t idx, const string& key);
    // 新增缓存,需要加锁
    static void addCache(size_t idx, const string& key, const string& value);
    // 更新同步缓存,第1和第4小步需要加锁
    static void updateCache();
    // 持久化保存
    static void writeToFile();
    static void destory();
private:
    CacheManager();
    ~CacheManager(){}
private:
    static CacheManager * _pInstance;
    size_t _cacheNum;
    string _cacheFileName;
    mutex _mutex;
    vector<Cache> _caches;
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

// 查询缓存,不需要上锁
inline string CacheManager::queryCache(size_t idx,const string& key){
    return getInstance()->_caches[idx].query(key);
}
// 新增缓存,需要加锁
inline void CacheManager::addCache(size_t idx, const string& key, const string& value){
    CacheManager * pcache = getInstance();
    unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
    pcache->_caches[idx].addRecord(key,value);
}

#endif //_CACHEMANAGER_H
