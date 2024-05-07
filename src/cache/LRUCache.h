#ifndef _LRUCACHE_H
#define _LRUCACHE_H

#include <string>
#include <list>
#include <unordered_map>
#include <wfrest/json.hpp>
using std::string;
using std::list;
using std::unordered_map;
using std::pair;

// 注意自己实现一遍拷贝构造,不然hashmap的迭代器会失效
class LRUCache {
    using JsonString = string;
    using HashMap = unordered_map<string,list<pair<string,JsonString>>::iterator>;
    friend class CacheManager;
public: 
    LRUCache(size_t capacity = 10);
    LRUCache(const string& filename,size_t capacity = 10);
    LRUCache(const LRUCache & lhs); // 拷贝构造
    LRUCache& operator=(const LRUCache & lhs); // 拷贝函数
    JsonString query(const string& key); // 查询
    void addRecord(const string& key, const JsonString & jsonStr); // 插入结点
    void update(const LRUCache & cache); // 同步pendings到传入的cache
    /* list<pair<string,JsonString>>& getPendingUpdateList(){ return _pendingNodes; } */
    /* void prints(); // debug用 */
private:
    void readFromFile(const string& filename); // 缓存从文件初始化
    void writeToFile(const string& filename); // 缓存持久化
private:
    size_t _capacity;
    list<pair<string,JsonString>> _cacheNodes; // 缓存
    HashMap _hashmap; // 通过hash表查询缓存链表结点
    list<pair<string,JsonString>> _pendingNodes; // 待更新队列
};

// 构造析构
inline LRUCache::LRUCache(size_t capacity)
:_capacity(capacity)
{}
inline LRUCache::LRUCache(const string& filename,size_t capacity)
:_capacity(capacity)
{
    readFromFile(filename);
}

#endif //_LRUCACHE_H
