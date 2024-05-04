#include <iostream>
#include <sstream>
#include <fstream>
#include "../shared/Configuration.h"
#include "../shared/Mylogger.h"
#include "PageLib.h"
using std::ostringstream;
using std::ofstream;
using std::cout;

// 创建网页库、网页偏移库
void PageLib::create() {
    // 1 创建网页库
    _dirScanner.traverse(_pconf->getConfigMap()["xml_files"]);
    vector<string> files = _dirScanner.getFiles();
    long total = 0;// 总大小-->pos
    size_t docId = 0;// 总id
    for(auto const & file:files){
        // 1.1 遍历并解析所有xml文件
        vector<string> temp = _fileProcessor.process(file);
        for(auto const & page:temp){
            // 1.2 生成单个网页
            _pages.emplace_back(page);
            // 1.3 生成对应的偏移库
            size_t pageSize = page.size();
            _offsetLib[docId] = pair<long,long>{total,pageSize};
            ++docId;
            total += pageSize;
        }
    }
    /* for(size_t i = 0; i < _pages.size(); ++i){ */
    /*     cout << _pages[i]; */
    /*     cout << "id = " << i << "\n" */
    /*          << "pos = " << _offsetLib[i].first << "\n" */
    /*          << "len = " << _offsetLib[i].second << "\n"; */
    /* } */
    /* cout << _pages.size() << "\n"; */
    /* cout << total << "\n"; */
}

// 存储
void PageLib::store() {
    string webpageLib_old = _pconf->getConfigMap()["webpageLib_old"];
    string offsetLib_old = _pconf->getConfigMap()["offsetLib_old"];
    // 1 存储网页库
    ofstream ofs(webpageLib_old);
    if(!ofs){
        LogError("ofstream open webpageLib_old failed");
        return;
    }
    for(auto const & page:_pages){
        ofs << page;
    }
    ofs.close();
    // 2 存储网页偏移库
    ofs.open(offsetLib_old);
    if(!ofs){
        LogError("ofstream open offsetLib_old failed");
        return;
    }
    for(auto const & offset:_offsetLib){
        ofs << offset.first << " " << offset.second.first << " " << offset.second.second << "\n";
    }
    ofs.close();
}
