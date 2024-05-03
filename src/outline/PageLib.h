#ifndef _PAGELIB_H
#define _PAGELIB_H

#include <map>
#include "DirScanner.h"
#include "FileProcessor.h"
using std::map;
using std::pair;

class Configuration;
class PageLib {
public: 
    PageLib(Configuration * pconf):_pconf(pconf){}
    // 创建网页库、网页偏移库
    void create();
    // 存储
    void store();
private:
    Configuration * _pconf;
    DirScanner _dirScanner;
    FileProcessor _fileProcessor;
    vector<string> _pages;
    map<size_t, pair<long,long>> _offsetLib;
};

#endif //_PAGELIB_H
