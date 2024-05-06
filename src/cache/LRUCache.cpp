#include "LRUCache.h"

// 查询操作
LRUCache::JsonString LRUCache::query(const string& key){
    // 1.1 命中 & 将节点调整至链表开头
    if(_hashmap.count(key)){
        auto result = _hashmap[key];
        _cacheNodes.splice(_cacheNodes.begin(),_cacheNodes,result);
        _pendingNodes.splice(_pendingNodes.begin(),_pendingNodes,_pendingHashmap[key]);
        return result->second;
    }
    // 1.2 未命中
    return "";
}

// 插入节点 & 更新队列 (两者保持一致)
void LRUCache::addRecord(const string& key, const JsonString & jsonStr){
    _cacheNodes.emplace_front(key,jsonStr);
    _hashmap[key] = _cacheNodes.begin();
    _pendingNodes.emplace_front(key,jsonStr);
    _pendingHashmap[key] = _pendingNodes.begin();
    if(_cacheNodes.size() > _capacity){
        _hashmap.erase(_cacheNodes.rbegin()->first);
        _cacheNodes.pop_back();
        _pendingHashmap.erase(_pendingNodes.rbegin()->first);
        _pendingNodes.pop_back();
    }
}

// 同步为传入的缓存
void LRUCache::update(const LRUCache & cache){

}

// 缓存从文件初始化
void LRUCache::readFromFile(const string& filename){

}

// 缓存持久化
void LRUCache::writeToFile(const string& filename){

}

