#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H

#include <unordered_map>
#include <queue>
#include "../shared/Configuration.h"
#include "../shared/WebPage.h"
#include "../shared/SplitTool.h"
using std::pair;
using std::unordered_map;
using std::priority_queue;

// 用于优先级队列比较
struct CosCompare{
    bool operator()(pair<size_t,double>& lhs,pair<size_t,double>& rhs) const{
        return lhs.second < rhs.second;
    }
};

class WebPageQuery {
public: 
    static WebPageQuery * getInstance();
    static void init(Configuration * pconf);
    static string doQuery(const string& key);
private:
    WebPageQuery();
    static void destory();
private:
    static WebPageQuery * _pInstance;
    SplitToolCppJieba _cutChinese;
    vector<WebPage> _pages;
    unordered_map<size_t,pair<long,long>> _offsetLib;
    unordered_map<string,map<size_t,double>> _invertIndexLib;
    priority_queue<pair<size_t,double>,std::vector<pair<size_t,double>>,CosCompare> _result;
};

inline WebPageQuery::WebPageQuery()
:_cutChinese(Configuration::getInstance())
{
    // 自动销毁
    ::atexit(&WebPageQuery::destory);
}

#endif //_WEBPAGEQUERY_H
