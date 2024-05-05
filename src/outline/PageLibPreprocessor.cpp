#include <unordered_set>
#include <functional>
#include <wfrest/json.hpp>
#include "../shared/tinyxml2.h"
#include "../shared/Mylogger.h"
#include "../shared/Configuration.h"
#include "../shared/SplitTool.h"
#include "PageLibPreprocessor.h"
using namespace std::placeholders;
using namespace tinyxml2;
using namespace simhash;
using std::unique_ptr;

// 用于simhash计算使用
const size_t topN = 10;
// 用于分块处理doc
const size_t docNum = 100;
// 用于unordered_set比较
struct MyCompare{
    bool operator()(int64_t lhs, int64_t rhs)const{
        return Simhasher::isEqual(lhs,rhs,3);
    }
};

// 构造函数
PageLibPreprocessor::PageLibPreprocessor(Configuration * pconf,SplitTool * ptool)
:_pconf(pconf)
,_pWordCutter(ptool)
,_pSimhasher(creatSimhasher())
{}
Simhasher * PageLibPreprocessor::creatSimhasher(){
    auto configs = _pconf->getConfigMap();
    const char* const DICT_PATH = configs["DICT_PATH"].c_str();
    const char* const HMM_PATH = configs["HMM_PATH"].c_str();
    const char* const IDF_PATH = configs["IDF_PATH"].c_str();
    const char* const STOP_WORD_PATH = configs["STOP_WORD_PATH"].c_str();
    return new Simhasher(DICT_PATH,HMM_PATH,IDF_PATH,STOP_WORD_PATH);
}

// 去重 & 计算simhash指纹 & 构建网页库
void PageLibPreprocessor::cutRedundantPage() {
    // 获取配置文件
    auto configs = _pconf->getConfigMap();
    string webpageLib_old = configs["webpageLib_old"];
    string offsetLib_old = configs["offsetLib_old"];
    // 读取offsetLib_old
    ifstream ifs(offsetLib_old);
    if(!ifs){
        LogError("open offsetLib_old file failed");
        return;
    }
    while(ifs){
        size_t docId;
        long pos,len;
        ifs >> docId >> pos >> len;
        _offsetLib.emplace_back(pos,len);
    }
    ifs.close();
    // 读取webpageLib_old,根据网页偏移库分块处理
    ifs.open(webpageLib_old);
    if(!ifs){
        LogError("open webpageLib_old file failed");
        return;
    }
    unordered_set<int64_t,std::hash<int64_t>,MyCompare> hashs; // 储存已计算出的simhash,用于去重
    for(size_t i = 0; i < _offsetLib.size(); i += docNum){
        ifs.seekg(_offsetLib[i].first); // 偏移位置
        size_t docsSize = 0; // 获取要读取的大小
        for(size_t j = 0; j < docNum && i+j < _offsetLib.size()-1; j++){
            docsSize += _offsetLib[i+j].second;
        }
        /* cout << docsSize << "\n"; */
        char buf[docsSize + 1];
        buf[docsSize] = 0;
        ifs.read(buf,docsSize);
        // 1 解析doc
        XMLDocument doc;
        doc.Parse(buf);
        // 2检查报错
        if(doc.ErrorID()){
            LogError("XML Parse failed");
            cout << doc.ErrorStr() << "\n";
            return;
        }
        // 3处理
        // 3.1获取第一个doc
        XMLElement * pItemNode = doc.FirstChildElement("doc");
        // 3.2循环处理所有doc
        while(pItemNode){
            string title,link,content;
            XMLElement * temp = pItemNode->FirstChildElement("content");
            // 跳过坏数据
            if(temp == nullptr){
                pItemNode = pItemNode->NextSiblingElement("doc");
                continue;
            }
            if(temp->GetText() == nullptr){
                pItemNode = pItemNode->NextSiblingElement("doc");
                continue;
            }
            content = temp->GetText();
            // 替换"<"
            size_t pos = 0;
            while ((pos = content.find("<", pos)) != std::string::npos) {
                content.replace(pos, 1, "〈");
                pos += 3; // 移动到下一个位置，避免替换后死循环
            }
            // 替换">"
            pos = 0;
            while ((pos = content.find(">", pos)) != std::string::npos) {
                content.replace(pos, 1, "〉");
                pos += 3; // 移动到下一个位置，避免替换后死循环
            }
            temp = pItemNode->FirstChildElement("title");
            if(temp != nullptr){
                title = temp->GetText();
            }
            temp = pItemNode->FirstChildElement("link");
            if(temp != nullptr){
                link = temp->GetText();
            }
            /* cout << title << "\n"; */
            /* cout << link << "\n"; */
            /* cout << content << "\n"; */
            // 计算simhash
            vector<pair<string ,double>> res; // 关键词序列
            uint64_t u64 = 0; // simhash指纹
            _pSimhasher->extract(content, res, topN);
            _pSimhasher->make(content, topN, u64);
            // 插入vector
            if(hashs.count(u64)){
                // 已存在,去重
                pItemNode = pItemNode->NextSiblingElement("doc");
                continue;
            }
            hashs.insert(u64);
            _pageLib.emplace_back(title,link,content);
            // 获取下一个doc
            pItemNode = pItemNode->NextSiblingElement("doc");
            /* cout << _pageLib.size() << "\n\n"; */
        }
    }
    ifs.close();
#if 0
    // 1加载xml文件
    XMLDocument doc;
    doc.LoadFile(webpageLib_old.c_str());
    // 2检查报错
    if(doc.ErrorID()){
        LogError("XML loadfile failed");
        return;
    }
    // 3处理
    // 3.1获取第一个doc
    XMLElement * pItemNode = doc.FirstChildElement("doc");
    // 3.2循环处理所有doc
    unordered_set<int64_t,std::hash<int64_t>,MyCompare> hashs; // 储存已计算出的simhash,用于去重
    while(pItemNode){
        string title,link,content;
        title = pItemNode->FirstChildElement("title")->GetText();
        link = pItemNode->FirstChildElement("link")->GetText();
        content = pItemNode->FirstChildElement("content")->GetText();
        // 计算simhash
        vector<pair<string ,double>> res; // 关键词序列
        uint64_t u64 = 0; // simhash指纹
        _pSimhasher->extract(content, res, topN);
        _pSimhasher->make(content, topN, u64);
        // 插入vector
        if(hashs.count(u64)){
            // 已存在,去重
            pItemNode = pItemNode->NextSiblingElement("doc");
            continue;
        }
        hashs.insert(u64);
        _pageLib.emplace_back(title,link,content);
        // 获取下一个doc
        pItemNode = pItemNode->NextSiblingElement("doc");
    }
#endif
}

// 构建倒排索引
void PageLibPreprocessor::bulidInvertIndexMap() {
    size_t pageNum = _pageLib.size(); // 网页数量
    const unordered_set<string>& stopWords = _pconf->getStopWords(); // 停用词
    // 总共三次循环遍历
    // 1. 统计文章内词频TF & 统计多少文章出现过DF & 构建倒排索引第一版(string & map<docId,?>)
    map<string,size_t> docCounts; // 多少文章出现过DF
    vector<vector<string>> docWords(pageNum); // 每篇文档的所有单词
    for(size_t i = 0; i < pageNum; ++i){
        map<string,size_t> wordCounts; // 词频统计
        // 分词
        vector<string> temp = _pWordCutter->cutWord(_pageLib[i]._content);
        // 统计
        for(auto & word:temp){
            if(stopWords.count(word)){
                continue; // 停用词跳过
            }
            ++wordCounts[word]; // 词频统计
        }
        // 记录
        docWords[i].reserve(wordCounts.size());
        for(auto & word:wordCounts){
            docWords[i].push_back(word.first); // 记录所有不重复的单词
            ++docCounts[word.first]; // 章频统计
            _invertIndexLib[word.first][i] = wordCounts[word.first]; // 倒排索引 & 暂存TF
        }
    }
    // 2. 计算TF & IDF & w权重
    //    TF:词频  IDF:log2(pageNum/(章频DF+1))  w:TF * IDF
    for(size_t i = 0; i < pageNum; ++i){
        const vector<string>& words = docWords[i];
        for(size_t j = 0; j < words.size(); ++j){
            const string& word = words[j];
            double tf = _invertIndexLib[word][i]; // 取出暂存的TF
            double df = docCounts[word]; // 取出DF
            _invertIndexLib[word][i] = tf * ::log2(pageNum/(df + 1)); // 计算权重w
        }
    }
    // 3. 权重w归一化,防止大文档所有的词权重过高
    for(size_t i = 0; i < pageNum; ++i){
        const vector<string>& words = docWords[i];
        double totalW = 0;
        for(size_t j = 0; j < words.size(); ++j){
            const string& word = words[j];
            totalW += ::pow(_invertIndexLib[word][i],2);
        }
        totalW = ::sqrt(totalW);
        for(size_t j = 0; j < words.size(); ++j){
            const string& word = words[j];
            _invertIndexLib[word][i] /= totalW;
        }
    }
}

// 存储到磁盘文件
// 网页库:xml 网页偏移库:docId,pos,len为一行 倒排索引库:string int+w&int+w...为一行
void PageLibPreprocessor::storeOnDisk() {
    // 1 获取配置文件
    auto configs = _pconf->getConfigMap();
    string webpageLib = configs["webpageLib"];
    string offsetLib = configs["offsetLib"];
    string invertIndexLib = configs["invertIndexLib"];
    // 2 网页库 & 网页偏移库
    ofstream ofs1(webpageLib);
    if(!ofs1){
        LogError("open webpageLib failed");
        return;
    }
    ofstream ofs2(offsetLib);
    if(!ofs2){
        LogError("open offsetLib failed");
        return;
    }
    // 2.1 遍历保存
    size_t pageNum = _pageLib.size();
    long totalSize = 0; // pos
    for(size_t i = 0; i < pageNum; ++i){
        ostringstream oss;
        oss << "<doc>\n<title>" << _pageLib[i]._title
            << "</title>\n<link>" << _pageLib[i]._link
            << "</link>\n<content>" << _pageLib[i]._content
            << "</content>\n</doc>\n";
        string resultPage = oss.str();
        ofs1 << resultPage;
        ofs2 << i << " " << totalSize << " " << resultPage.size() << "\n";
        totalSize += resultPage.size();
        /* cout << i << " " << totalSize << " " << resultPage.size() << "\n"; */
    }
    ofs1.close();
    ofs2.close();
    // 3 倒排索引库
    // unordered_map<string,map<size_t,double>> _invertIndexLib;
    ofs1.open(invertIndexLib);
    if(!ofs1){
        LogError("open invertIndexLib failed");
        return;
    }
    /* nlohmann::json root; */
    for(const auto& entry : _invertIndexLib){
        /* nlohmann::json inner_map; */
        ofs1 << entry.first << " ";
        for(const auto& inner_entry : entry.second){
            /* inner_map[std::to_string(inner_entry.first)] = inner_entry.second; */
            ofs1 << inner_entry.first << " " << inner_entry.second << " ";
        }
        /* root[entry.first] = inner_map; */
        ofs1 << "\n";
    }
    ofs1.close();
}
