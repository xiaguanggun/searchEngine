#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <queue>
#include "../shared/SplitTool.h"
#include "CandidateResult.hpp"
using std::string;
using std::vector;
using std::pair;
using std::unordered_map;
using std::set;
using std::priority_queue;

class Configuration;
class Dictionary {
public:
    static Dictionary * getInstance();
    static void init(Configuration * pconf);
    static string doQuery(const string& key);
private: 
    Dictionary();
    ~Dictionary(){}
    static void destory();
    /* void queryIndex(); */
    /* int distance(string candidate); */
private: 
    static Dictionary * _pInstance;
    SplitToolChar _cutChar;
    vector<pair<string,size_t>> _dictEng;
    vector<pair<string,size_t>> _dictZh;
    unordered_map<string,set<size_t>> _indexEng;
    unordered_map<string,set<size_t>> _indexZh;
    size_t _pqSize = 10;
    priority_queue<CandidateResult> _result;
};

inline Dictionary::Dictionary(){
    // 自动销毁
    ::atexit(&Dictionary::destory);
}

#endif //_DICTIONARY_H
