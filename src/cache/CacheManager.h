#ifndef _CACHEMANAGER_H
#define _CACHEMANAGER_H

#include <vector>
#include <mutex>
#include "LRUCache.h"
#include "../shared/Configuration.h"
using std::vector;
using std::mutex;
using std::unique_lock;

class CacheManager {
public: 
    static CacheManager * getInstance();
    static void init(size_t threadNum);
    // 查询缓存
    static string queryKeyWordCache(size_t idx, const string& key);
    static string queryWebQueryCache(size_t idx, const string& key);
    // 新增缓存,需要加锁
    static void addKeyWordCache(size_t idx, const string& key, const string& value);
    static void addWebQueryCache(size_t idx, const string& key, const string& value);
    /* static LRUCache& getKeyWordCache(size_t idx); */
    /* static LRUCache& getWebQueryCache(size_t idx); */
    // 更新同步缓存,第1和第4小步需要加锁
    static void updateCache();
    // 持久化保存
    static void writeToFile();
private:
    CacheManager();
    ~CacheManager(){}
    static void destory();
private:
    static CacheManager * _pInstance;
    size_t _threadNum;
    mutex _mutex;
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

// 查询缓存,不需要上锁
inline string CacheManager::queryKeyWordCache(size_t idx,const string& key){
    return getInstance()->_cachesKeyWord[idx].query(key);
}
inline string CacheManager::queryWebQueryCache(size_t idx, const string& key){
    return getInstance()->_cachesWebQuery[idx].query(key);
}
// 新增缓存,需要加锁
inline void CacheManager::addKeyWordCache(size_t idx, const string& key, const string& value){
    CacheManager * pcache = getInstance();
    unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
    pcache->_cachesKeyWord[idx].addRecord(key,value);
}
inline void CacheManager::addWebQueryCache(size_t idx, const string& key, const string& value){
    CacheManager * pcache = getInstance();
    unique_lock<mutex> autolock(pcache->_mutex);// 自动解锁
    pcache->_cachesKeyWord[idx].addRecord(key,value);
}

#if 0
// 获取Cache
inline LRUCache& CacheManager::getKeyWordCache(size_t idx){
    return getInstance()->_cachesKeyWord[idx];
}
inline LRUCache& CacheManager::getWebQueryCache(size_t idx){
    return getInstance()->_cachesWebQuery[idx];
}
#endif

#endif //_CACHEMANAGER_H
