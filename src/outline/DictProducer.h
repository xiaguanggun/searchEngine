#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H

#include <set>
#include <unordered_map>
#include "../shared/Configuration.h"
#include "../shared/SplitTool.h"
using std::set;
using std::pair;
using std::unordered_map;

class DictProducer {
public: 
    DictProducer(const string& dir_eng,SplitTool * pChar); // 处理英文
    DictProducer(const string& dir_zh,SplitTool * pChar,SplitTool * pZh); // 处理中文
    // 根据_cutChinese指针是否为nullptr决定构造eng/zhDict
    void buildDict();
    void createIndex();
    void store();
private:
    void buildEnDict();
    void buildCnDict();
    vector<string> getFiles(const string& dir);
private: 
    vector<string> _files;
    vector<pair<string,size_t>> _dict;
    unordered_map<string,set<size_t>> _index;
    SplitTool* _cutChar;
    SplitTool* _cutChinese;
};

inline void DictProducer::buildDict(){
    if(_cutChinese == nullptr){
        buildEnDict();
    }else{
        buildCnDict();
    }
}

#endif //_DICTPRODUCER_H
