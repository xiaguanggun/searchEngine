#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
using std::vector;
using std::string;
using std::set;
using std::pair;
using std::unordered_map;

class SplitTool;
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
    SplitTool* _pcutChar;
    SplitTool* _pcutChinese;
};

inline void DictProducer::buildDict(){
    if(_pcutChinese == nullptr){
        buildEnDict();
    }else{
        buildCnDict();
    }
}

#endif //_DICTPRODUCER_H
