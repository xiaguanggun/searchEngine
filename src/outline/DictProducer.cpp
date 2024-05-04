#include <sstream>
#include <iterator>
#include "../shared/Configuration.h"
#include "../shared/SplitTool.h"
#include "DirScanner.h"
#include "DictProducer.h"
using std::istringstream;
using std::ofstream;

// 构造函数
DictProducer::DictProducer(const string& dir_eng,SplitTool * pChar)
:_files(getFiles(dir_eng))
,_cutChar(pChar)
,_cutChinese(nullptr)
{}
DictProducer::DictProducer(const string& dir_zh,SplitTool * pChar,SplitTool * pZh)
:_files(getFiles(dir_zh))
,_cutChar(pChar)
,_cutChinese(pZh)
{}
vector<string> DictProducer::getFiles(const string& dir){
    DirScanner scanner;
    scanner.traverse(dir);
    return scanner.getFiles();
}

void DictProducer::createIndex() {
    // 遍历_dict & 切分字符 & 构造索引
    for(size_t i = 0; i < _dict.size(); ++i){
        vector<string> chars = _cutChar->cutWord(_dict[i].first);
        for(auto& ch:chars){
            _index[ch].insert(i);
        }
    }
}

void DictProducer::store() {
    // 根据_cutChinese指针是否为nullptr决定构造eng/zhDict
    string dict_filename,idx_filename;
    if(_cutChinese == nullptr){
        dict_filename = Configuration::getConfigMap()["dict_eng"];
        idx_filename = Configuration::getConfigMap()["dict_eng_idx"];
    }
    else{
        dict_filename = Configuration::getConfigMap()["dict_zh"];
        idx_filename = Configuration::getConfigMap()["dict_zh_idx"];
    }
    // 存储词典 & 索引文件
    ofstream ofs(dict_filename);
    if(!ofs){
        LogError("open dict_file failed");
        return;
    }
    for(auto & word_pair:_dict){
        ofs << word_pair.first << " " << word_pair.second << "\n";
    }
    ofs.close();
    ofs.open(idx_filename);
    if(!ofs){
        LogError("open idx_file failed");
        return;
    }
    for(auto & char_pair:_index){
        ofs << char_pair.first << " ";
        for(auto & idx:char_pair.second){
            ofs << idx << " ";
        }
        ofs << "\n";
    }
}

// 辅助函数,中英文只有分词方法和日志信息不一样
void DictProducer::buildEnDict() {
    // 获取停用词
    unordered_set<string>& stop_words =  Configuration::getStopWords();
    // 遍历文件 & 分词 & 词频统计
    map<string,size_t> wordCounts; // 统计词频,最后写入到vector
    for(auto & file:_files){
        ifstream ifs(file);
        if(!ifs){
            LogError("open dict_files_eng failed");
            return;
        }
        string line;
        while(getline(ifs,line)){
            // 英文分词
            for(auto &ch:line){
                if(::ispunct(ch) || ::isdigit(ch)){ ch = ' '; } // 去掉标点符号 & 数字
                if(::isupper(ch)){ ch = ::tolower(ch); } // 变为全小写
            }
            istringstream iss(line);
            // 使用流迭代器插入vector
            vector<string> words{std::istream_iterator<string>(iss),std::istream_iterator<string>()};
            // 过滤停用词 & 统计词频
            for(auto& word:words){
                if(stop_words.count(word)){
                    continue;
                }
                ++wordCounts[word];
            }
        }
        ifs.close();
    }
    // 统计完成后写入_dict
    for(auto& word_pair:wordCounts){
        _dict.emplace_back(word_pair.first,word_pair.second);
    }
}

void DictProducer::buildCnDict() {
    // 获取停用词
    unordered_set<string>& stop_words =  Configuration::getStopWords();
    // 遍历文件 & 分词 & 词频统计
    map<string,size_t> wordCounts; // 统计词频,最后写入到vector
    for(auto & file:_files){
        ifstream ifs(file);
        if(!ifs){
            LogError("open dict_files_zh failed");
            return;
        }
        string line;
        while(getline(ifs,line)){
            // 中文分词
            vector<string> words = _cutChinese->cutWord(line);
            // 跳过数字/英文 & 过滤停用词 & 统计词频
            for(auto& word:words){
                if(((word[0] & 0x80) == 0) || stop_words.count(word)){
                    continue;
                }
                ++wordCounts[word];
            }
        }
        ifs.close();
    }
    // 统计完成后写入_dict
    for(auto& word_pair:wordCounts){
        _dict.emplace_back(word_pair.first,word_pair.second);
    }
}
