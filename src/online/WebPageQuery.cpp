#include <math.h>
#include <sstream>
#include <algorithm>
#include <wfrest/json.hpp>
#include "WebPageQuery.h"
#include "../shared/tinyxml2.h"
using std::istringstream;

// 静态成员
WebPageQuery * WebPageQuery::_pInstance = nullptr;
// 构造析构
WebPageQuery * WebPageQuery::getInstance(){
    if(!_pInstance){
        _pInstance = new WebPageQuery();
    }
    return _pInstance;
}
void WebPageQuery::destory(){
    if(_pInstance){
        delete _pInstance;
        _pInstance = nullptr;
    }
}

// 初始化
void WebPageQuery::init(Configuration * pconf){
    WebPageQuery * pweb = getInstance();
    auto & configs = pconf->getConfigMap();
    string line,words; // 获取一行内容 & 单词
    size_t idx; // 获取索引
    double w; // 获取权重
    long pos,len; // 获取网页偏移位置 & 网页长度
    ifstream ifs(configs["invertIndexLib"]);
    if(!ifs){
        LogError("open invertIndexLib failed");
        return;
    }
    // 加载倒排索引
    unordered_map<string,map<size_t,double>>& invertIndexLib = pweb->_invertIndexLib;
    while(getline(ifs,line)){
        istringstream iss(line);
        iss >> words;
        while(iss){
            iss >> idx >> w;
            invertIndexLib[words][idx] = w;
        }
    }
    ifs.close();
    ifs.open(configs["offsetLib"]);
    if(!ifs){
        LogError("open offsetLib failed");
        return;
    }
    ifstream ifs1(configs["webpageLib"]);
    if(!ifs1){
        LogError("open webpageLib failed");
        return;
    }
    // 加载网页偏移库 & 加载网页库 & 提取摘要(文章内容开头10个字)
    unordered_map<size_t,pair<long,long>>& offsetLib = pweb->_offsetLib;
    vector<WebPage>& pages= pweb->_pages;
    while(getline(ifs,line)){
        // 加载网页偏移库
        istringstream iss(line);
        iss >> idx >> pos >> len;
        offsetLib[idx] = pair<long,long>{pos,len};
        // 加载网页库 & 提取摘要(文章内容开头10个字)
        char buf[len + 1];
        buf[len] = 0;
        ifs1.seekg(pos); // 定位位置
        ifs1.read(buf,len); // 读取固定长度
        // 处理为xml文档格式
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(buf);
        if (result != tinyxml2::XML_SUCCESS) {
            LogError("Failed to parse XML string");
            return;
        }
        string title,link,content;
        tinyxml2::XMLElement * pItemNode = doc.FirstChildElement("doc");
        title = pItemNode->FirstChildElement("title")->GetText();
        link = pItemNode->FirstChildElement("link")->GetText();
        content = pItemNode->FirstChildElement("content")->GetText();
        /* content.resize(60); */
        /* content += "....."; */
        // 插入vector
        pages.emplace_back(title,link,content);
    }
    ifs.close();
    ifs1.close();
    // test
    //for(auto & word_pair:invertIndexLib){
    //    std::cout << word_pair.first << " ";
    //    for(auto & idx_pair:word_pair.second){
    //        std::cout << idx_pair.first << " " << idx_pair.second << " ";
    //    }
    //    std::cout << "\n";
    //}
    //for(auto & idx_pos_len:offsetLib){
    //    std::cout << idx_pos_len.first << " " << idx_pos_len.second.first << " " << idx_pos_len.second.second << "\n";
    //}
    //for(auto & page:pages){
    //    std::cout << page._content << "\n";
    //}
}

// 查询函数doQuery
string WebPageQuery::doQuery(const string& key){
    WebPageQuery * pweb = getInstance();
    const unordered_set<string>& stopWords = Configuration::getStopWords(); // 停用词
    const vector<WebPage>& pages = pweb->_pages;
    /* const unordered_map<size_t,pair<long,long>>& offsetLib = pweb->_offsetLib; */
    unordered_map<string,map<size_t,double>>& invertIndexLib = pweb->_invertIndexLib;
    priority_queue<pair<size_t,double>,std::vector<pair<size_t,double>>,CosCompare>& result = pweb->_result; // 存idx & cosSim
    // 切分字符
    const vector<string>& words = pweb->_cutChinese.cutWord(key);
    map<string,size_t> wordCounts; // 词频
    // 查索引获取网页下标集合 & 取交集 & 构造文档向量 & 基准向量(使用词频,IDF都一样为log2(1/2))
    map<size_t,double> idx_pairs = invertIndexLib[words[0]]; // 取交集后的map
    for(size_t i = 1; i < words.size(); ++i){
        const string& word = words[i];
        /* std::cout << word << "\n"; */
        if(stopWords.count(word)){
            continue; // 停用词跳过
        }
        ++wordCounts[word]; // 词频统计
        // 取交集
        map<size_t,double>& temp1 = invertIndexLib[word];
        map<size_t,double> temp2;
        idx_pairs.swap(temp2);
        std::set_intersection(temp1.begin(),temp1.end(),
                              temp2.begin(),temp2.end(),
                              inserter(idx_pairs,idx_pairs.begin()),
                              [](const pair<size_t,double>& lhs,
                                 const pair<size_t,double>& rhs) {
                                  return lhs.first < rhs.first;
                              }); // lambda用于取交集调用
        //std::cout << "size = " << idx_pairs.size() << "\n";
        //for(auto & it : idx_pairs){
        //    std::cout << it.first << " ";
        //}
        //std::cout << "\n";
        // 交集一旦为空则直接返回,未查到结果
        if(idx_pairs.size() == 0){
            /* std::cout << "no result\n"; */
            return "";
        }
    }
    // 构造基准向量
    vector<double> vQuery;
    double totalDoc = 0;
    for(auto& word_pair : wordCounts){
        vQuery.emplace_back(word_pair.second);
        totalDoc += ::pow(word_pair.second,2);
    }
    // 构造所有文档向量 & 计算cos相似度 & 插入优先级队列
    for(auto idx_pair:idx_pairs){
        size_t idx = idx_pair.first;
        // 构建向量
        vector<double> vDoc;
        double totalQuery = 0;
        for(auto& word_pair : wordCounts){
            // w = invertIndexLib[word][idx]
            double w = invertIndexLib[word_pair.first][idx];
            vDoc.emplace_back(w);
            totalQuery += ::pow(w,2);
        }
        // 计算cosSim余弦相似度
        double cosSim = 0;
        for(size_t i = 0; i < vQuery.size(); ++i){
            cosSim += vQuery[i] * vDoc[i];
        }
        cosSim /= ::sqrt(totalDoc * totalQuery);
        // 插入优先级队列
        result.emplace(idx,cosSim);
    }
    // 返回结果 json对象
    nlohmann::json queryResults;
    while(!result.empty()){
        size_t idx = result.top().first;
        const WebPage & page = pages[idx];
        nlohmann::json queryResult;
        queryResult["title"] = page._title;
        queryResult["link"] = page._link;
        queryResult["content"] = page._content;
        queryResults[std::to_string(idx)] = queryResult;
        result.pop();
    }
    return queryResults.dump();
}
