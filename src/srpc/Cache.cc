#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <workflow/WFTaskFactory.h>
#include <wfrest/json.hpp>
#include "../shared/Mylogger.h"
#include "Cache.h"
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;

// 拷贝构造
Cache::Cache(const Cache & lhs){
    _capacity = lhs._capacity;
    for(auto & it:lhs._cacheNodes){
        _cacheNodes.emplace_front(it.first,it.second);
        _hashmap[it.first] = _cacheNodes.begin();
    }
}
// 重载operator=
Cache& Cache::operator=(const Cache & lhs){
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
Cache::JsonString Cache::query(const string& key){
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
void Cache::addRecord(const string& key, const JsonString & jsonStr){
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
    std::cout << "nodes = " << _cacheNodes.size() << "\n";
}

// 同步传入的cache的pendNodes到自己
void Cache::update(const Cache & cache){
    for(auto & node : cache._pendingNodes){
        if(!_hashmap.count(node.first)){
            _cacheNodes.emplace_front(node.first,node.second);
            _hashmap[node.second] = _cacheNodes.begin();
        }
    }
}

// 缓存从文件初始化
void Cache::readFromFile(const string& filename){
    std::cout << "load Cache " << filename << "\n";
    ifstream ifs(filename);
    if(!ifs){
        LogInfo("load Cache file failed");
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
void Cache::writeToFile(const string& filename){
    std::cout << "save Cache " << filename << "\n";
    ofstream ofs(filename);
    if(!ofs){
        LogError("save Cache file failed");
        return;
    }
    for(auto & node : _cacheNodes){
        ofs << node.first << "\n" << node.second << "\n";
    }
    ofs.close();
}

