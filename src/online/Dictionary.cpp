#include <sstream>
#include <iterator>
#include <wfrest/json.hpp>
#include "../shared/Configuration.h"
#include "Dictionary.h"
using std::istringstream;
using std::istream_iterator;

//1. 求取一个字符占据的字节数
size_t nBytesCode(const char ch);
//2. 求取一个字符串的字符长度
std::size_t length(const std::string &str);
//3. 中英文通用的最小编辑距离算法
int editDistance(const std::string & lhs, const std::string &rhs);


// 静态成员
Dictionary * Dictionary::_pInstance = nullptr;
// 构造析构
Dictionary * Dictionary::getInstance(){
    if(!_pInstance){
        _pInstance = new Dictionary();
    }
    return _pInstance;
}
void Dictionary::destory(){
    if(_pInstance){
        delete _pInstance;
        _pInstance = nullptr;
    }
}
// 初始化
void Dictionary::init(Configuration * pconf){
    Dictionary * pdict = getInstance();
    auto & configs = pconf->getConfigMap();
    string line,chars; // 获取一行内容 & 单个字符或单词
    size_t tf; // 获取词频
    ifstream ifs(configs["dict_eng_idx"]);
    if(!ifs){
        LogError("open dict_eng_idx failed");
        return;
    }
    unordered_map<string,set<size_t>>& indexEng = pdict->_indexEng;
    while(getline(ifs,line)){
        istringstream iss(line);
        iss >> chars;
        indexEng[chars].insert(std::istream_iterator<size_t>(iss),std::istream_iterator<size_t>());
    }
    ifs.close();
    ifs.open(configs["dict_zh_idx"]);
    if(!ifs){
        LogError("open dict_zh_idx failed");
        return;
    }
    unordered_map<string,set<size_t>>& indexZh = pdict->_indexZh;
    while(getline(ifs,line)){
        istringstream iss(line);
        iss >> chars;
        indexZh[chars].insert(std::istream_iterator<size_t>(iss),std::istream_iterator<size_t>());
    }
    ifs.close();
    ifs.open(configs["dict_eng"]);
    if(!ifs){
        LogError("open dict_eng failed");
        return;
    }
    vector<pair<string,size_t>>& dictEng = pdict->_dictEng;
    while(ifs){
        ifs >> chars >> tf;
        dictEng.emplace_back(chars,tf);
    }
    ifs.close();
    ifs.open(configs["dict_zh"]);
    if(!ifs){
        LogError("open dict_zh failed");
        return;
    }
    vector<pair<string,size_t>>& dictZh = pdict->_dictZh;
    while(ifs){
        ifs >> chars >> tf;
        dictZh.emplace_back(chars,tf);
    }
    ifs.close();
    //test
    //for(auto & word_pair:pdict->_dictEng){
    //    std::cout << word_pair.first << " " << word_pair.second << "\n";
    //}
    //for(auto & char_pair:pdict->_indexEng){
    //    std::cout << char_pair.first << " ";
    //    for(auto & idx:char_pair.second){
    //        std::cout << idx << " ";
    //    }
    //    std::cout << "\n";
    //}
}

// 查询函数doQuery
string Dictionary::doQuery(const string& key) {
    Dictionary * pdict = getInstance();
    /* const unordered_set<string>& stopWords = Configuration::getStopWords(); // 停用词 */
    unordered_map<string,set<size_t>>& indexEng = pdict->_indexEng;
    unordered_map<string,set<size_t>>& indexZh = pdict->_indexZh;
    const vector<pair<string,size_t>>& dictEng = pdict->_dictEng;
    const vector<pair<string,size_t>>& dictZh = pdict->_dictZh;
    priority_queue<CandidateResult>& result = pdict->_result;
    // 切分字符
    const vector<string>& chars = pdict->_cutChar.cutWord(key);
    // 查索引获取单词下标集合 & 去重
    set<size_t> engSet,zhSet; // 去重后的set
    for(auto & ch:chars){
        /* if(stopWords.count(ch)){ */
        /*     continue; // 停用词跳过 */
        /* } */
        if((ch[0] & 0x80) == 0){
            // 英文
            set<size_t>& temp = indexEng[ch];
            engSet.insert(temp.begin(),temp.end());
        }
        else{
            // 中文
            set<size_t>& temp = indexZh[ch];
            zhSet.insert(temp.begin(),temp.end());
        }
    }
    // 获取候选词 & 计算最短编辑距离 & 存入优先级队列
    // 优先级队列改为小根堆(从小到大),当size超过_pqSize时就pop
    size_t pqSize = pdict->_pqSize;
    for(auto idx:engSet){
        auto & word_pair = dictEng[idx];
        /* if(result.size() == pqSize){ */
        /*     result.pop(); */
        /* } */
        result.emplace(word_pair.first,word_pair.second,editDistance(word_pair.first,key));
    }
    for(auto idx:zhSet){
        auto & word_pair = dictZh[idx];
        /* if(result.size() == pqSize){ */
        /*     result.pop(); */
        /* } */
        result.emplace(word_pair.first,word_pair.second,editDistance(word_pair.first,key));
    }
    // 返回优先级队列的倒序内容(从大到小)
    /* vector<string> temp; */
    /* for(size_t i = 0; i < result.size(); i++){ */
    /*     temp.emplace_back(result.top()._word); */
    /*     result.pop(); */
    /* } */
    /* return string(temp.rbegin(),temp.rend()); */
    /* string queryResult; */
    /* for(auto it = temp.rbegin(); it != temp.rend(); ++it){ */
    /*     queryResult += *it; */
    /*     queryResult += " "; */
    /* } */
    // 构造json对象
    nlohmann::json queryResult;
    queryResult["KeyWord"] = nlohmann::json::array(); // 数组
    for(size_t i = 0; i < pqSize && !result.empty(); ++i){
        queryResult["KeyWord"].push_back(result.top()._word);
        result.pop();
    }
    priority_queue<CandidateResult> temp;
    result.swap(temp);
    return queryResult.dump() + "\n";
}

// 私有辅助函数
/* void Dictionary::queryIndex() { */
/*     return; */
/* } */
// 计算最小编辑距离
/* int Dictionary::distance(string candidate) { */
/*     return 0; */
/* } */


// 本文件使用的函数
size_t nBytesCode(const char ch)
{
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;	
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}  
std::size_t length(const std::string &str)
{
	std::size_t ilen = 0;
	for(std::size_t idx = 0; idx != str.size(); ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}
int triple_min(const int &a, const int &b, const int &c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}
int editDistance(const std::string & lhs, const std::string &rhs)
{
    //计算最小编辑距离-包括处理中英文
	size_t lhs_len = length(lhs);
	size_t rhs_len = length(rhs);
	int editDist[lhs_len + 1][rhs_len + 1];
	for(size_t idx = 0; idx <= lhs_len; ++idx)
	{
		editDist[idx][0] = idx;
	}

	for(size_t idx = 0; idx <= rhs_len; ++idx)
	{
		editDist[0][idx] = idx;
	}
	
	std::string sublhs, subrhs;
	for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
	{
		size_t nBytes = nBytesCode(lhs[lhs_idx]);
		sublhs = lhs.substr(lhs_idx, nBytes);
		lhs_idx += (nBytes - 1);

		for(std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
		{
			nBytes = nBytesCode(rhs[rhs_idx]);
			subrhs = rhs.substr(rhs_idx, nBytes);
			rhs_idx += (nBytes - 1);
			if(sublhs == subrhs)
			{
				editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
			}
			else
			{
				editDist[dist_i][dist_j] = triple_min(
					editDist[dist_i][dist_j - 1] + 1,
					editDist[dist_i - 1][dist_j] + 1,
					editDist[dist_i - 1][dist_j - 1] + 1);
			}
		}
	}
	return editDist[lhs_len][rhs_len];
}

