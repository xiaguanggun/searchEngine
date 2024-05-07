#include <iostream>
#include <fstream>
#include <sstream>
#include <wfrest/json.hpp>
#include "../shared/Mylogger.h"
#include "LRUCache.h"
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istringstream;

#if 0
void LRUCache::prints(){
    for(auto & it:_hashmap){
        ::printf("%p\n",&(*it.second));
    }
    for(auto & it:_cacheNodes){
        ::printf("%p\n",&(it));
    }
    for(auto & it:_hashmap){
        cout << it.first << " " << it.second->second << "\n";
    }
    for(auto & it:_cacheNodes){
        cout << it.first << " " << it.second << "\n";
    }
}
#endif

// 拷贝构造
LRUCache::LRUCache(const LRUCache & lhs){
    for(auto & it:lhs._cacheNodes){
        _cacheNodes.emplace_front(it.first,it.second);
        _hashmap[it.first] = _cacheNodes.begin();
    }
}
// 重载operator=
LRUCache& LRUCache::operator=(const LRUCache & lhs){
    // 清空自己的内存
    _cacheNodes.clear();
    _hashmap.clear();
    _pendingNodes.clear();
    // 循环插入
    for(auto & it:lhs._cacheNodes){
        _cacheNodes.emplace_front(it.first,it.second);
        _hashmap[it.first] = _cacheNodes.begin();
    }
    return *this;
}

// 查询操作
LRUCache::JsonString LRUCache::query(const string& key){
    // 1.1 命中 & 将节点调整至链表开头
    if(_hashmap.count(key)){
        std::cout << "缓存命中\n";
        auto & result = _hashmap[key];
        // 为头结点时,不做操作
        if(result != _cacheNodes.begin()){
            _cacheNodes.splice(_cacheNodes.begin(),_cacheNodes,result);
        }
        return result->second;
    }
    // 1.2 未命中
    return "";
}

// 插入节点 & 待更新队列
void LRUCache::addRecord(const string& key, const JsonString & jsonStr){
    _cacheNodes.emplace_front(key,jsonStr);
    _hashmap[key] = _cacheNodes.begin();
    _pendingNodes.emplace_front(key,jsonStr);
    if(_cacheNodes.size() > _capacity){
        _hashmap.erase(_cacheNodes.rbegin()->first);
        _cacheNodes.pop_back();
    }
    if(_pendingNodes.size() > _capacity){
        _pendingNodes.pop_back();
    }
    // test
    /* std::cout << _cacheNodes.size() << "\n"; */
}

// 同步传入的cache的pendNodes到自己
void LRUCache::update(const LRUCache & cache){
    for(auto & node : cache._pendingNodes){
        if(!_hashmap.count(node.first)){
            _cacheNodes.emplace_front(node.first,node.second);
            _hashmap[node.second] = _cacheNodes.begin();
        }
    }
}

// 缓存从文件初始化
void LRUCache::readFromFile(const string& filename){
    std::cout << "load LRUCache" << filename << "\n";
    ifstream ifs(filename);
    if(!ifs){
        LogInfo("load LRUCache file failed");
        return;
    }
    while(ifs){
        string key,value;
        getline(ifs,key);
        getline(ifs,value);
        if(key.size() && value.size()){
            _cacheNodes.emplace_front(key,value);
            _hashmap[key] = _cacheNodes.begin();
        }
    }
    ifs.close();
}

// 缓存持久化
void LRUCache::writeToFile(const string& filename){
    std::cout << "save LRUCache" << filename << "\n";
    ofstream ofs(filename);
    if(!ofs){
        LogError("save LRUCache file failed");
        return;
    }
    for(auto & node : _cacheNodes){
        ofs << node.first << "\n" << node.second << "\n";
    }
    ofs.close();
}

