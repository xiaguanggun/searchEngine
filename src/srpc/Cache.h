#ifndef _CACHE_H
#define _CACHE_H

#include <string>
#include <list>
#include <unordered_map>
#include <wfrest/json.hpp>
using std::string;
using std::list;
using std::unordered_map;
using std::pair;

// 注意自己实现一遍拷贝构造,不然hashmap的迭代器会失效
class Cache {
    using JsonString = string;
    using HashMap = unordered_map<string,list<pair<string,JsonString>>::iterator>;
    friend class CacheManager;
public: 
    Cache(size_t capacity = 10);
    Cache(const string& filename,size_t capacity = 10);
    Cache(const Cache & lhs); // 拷贝构造
    /* Cache(Cache && lhs); // 移动构造 */
    Cache& operator=(const Cache & lhs); // 赋值函数
    /* Cache& operator=(const Cache && lhs); // 移动赋值函数 */

    JsonString query(const string& key); // 查询
    void addRecord(const string& key, const JsonString & jsonStr); // 插入结点
    void update(const Cache & cache); // 同步pendings到传入的cache
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
inline Cache::Cache(size_t capacity)
:_capacity(capacity)
{}
inline Cache::Cache(const string& filename,size_t capacity)
:_capacity(capacity)
{
    readFromFile(filename);
}

#endif //_Cache_H
