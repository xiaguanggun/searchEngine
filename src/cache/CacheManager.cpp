#include "CacheManager.h"

// 静态变量
CacheManager * CacheManager::_pInstance = nullptr;

// 初始化
void CacheManager::init(const string& filename, size_t threadNum) {
    /* CacheManager * pCache = getInstance(); */
    /* pCache->_caches.reserve(threadNum + 1); */
    vector<LRUCache>& caches = getInstance()->_caches;
    LRUCache temp(filename);
    caches.assign(threadNum + 1,temp);
}

// 同步更新所有缓存
void CacheManager::updateCache() {
    return;
}
