#ifndef _PAGELIBPREPROCESSOR_H
#define _PAGELIBPREPROCESSOR_H

#include <vector>
#include <unordered_map>
#include <memory>
#include "../../include/simhash/Simhasher.hpp"
#include "WebPage.h"
using std::vector;
using std::unordered_map;
using std::pair;
using std::unique_ptr;

class Configuration;
class SplitTool;
class PageLibPreprocessor {
public:
    PageLibPreprocessor(Configuration * pconf,SplitTool * ptool);
    // 去重 & 计算simhash指纹 & 构建网页库
    void cutRedundantPage();
    // 构建倒排索引
    void bulidInvertIndexMap();
    // 存储到磁盘文件
    void storeOnDisk();
private:
    simhash::Simhasher * creatSimhasher();
private:
    Configuration* _pconf;
    SplitTool* _pWordCutter;
    unique_ptr<simhash::Simhasher> _pSimhasher; // 自己创建Simhasher,自己销毁
    vector<WebPage> _pageLib;
    //vector<pair<long,long>> _offsetLib; // 根据old网页偏移库分块处理old网页库
    unordered_map<string,map<size_t,double>> _invertIndexLib;
};

#endif //_PAGELIBPREPROCESSOR_H
