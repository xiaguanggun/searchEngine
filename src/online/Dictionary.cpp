#include <sstream>
#include <iterator>
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
    while(getline(ifs,line)){
        istringstream iss(line);
        iss >> chars;
        pdict->_indexEng[chars].insert(std::istream_iterator<size_t>(iss),std::istream_iterator<size_t>());
    }
    ifs.close();
    ifs.open(configs["dict_zh_idx"]);
    if(!ifs){
        LogError("open dict_zh_idx failed");
        return;
    }
    while(getline(ifs,line)){
        istringstream iss(line);
        iss >> chars;
        pdict->_indexZh[chars].insert(std::istream_iterator<size_t>(iss),std::istream_iterator<size_t>());
    }
    ifs.close();
    ifs.open(configs["dict_eng"]);
    if(!ifs){
        LogError("open dict_eng failed");
        return;
    }
    while(ifs){
        ifs >> chars >> tf;
        pdict->_dictEng.emplace_back(chars,tf);
    }
    ifs.close();
    ifs.open(configs["dict_zh"]);
    if(!ifs){
        LogError("open dict_zh failed");
        return;
    }
    while(ifs){
        ifs >> chars >> tf;
        pdict->_dictZh.emplace_back(chars,tf);
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

// 成员函数
string Dictionary::doQuery(const string& key) {
    Dictionary * pdict = getInstance();
    // 切分字符
    vector<string> chars = pdict->_cutChar.cutWord(key);
    // 查索引获取单词下标集合 & 去重
    set<size_t> engSet,zhSet; // 去重后的set
    for(auto & ch:chars){
        if((ch[0] & 0x80) == 0){
            // 英文
            set<size_t>& temp = pdict->_indexEng[ch];
            engSet.insert(temp.begin(),temp.end());
        }
        else{
            // 中文
            set<size_t>& temp = pdict->_indexZh[ch];
            zhSet.insert(temp.begin(),temp.end());
        }
    }
    // 获取候选词 & 计算最短编辑距离 & 存入优先级队列
    // 优先级队列改为小根堆(从小到大),当size超过_pqSize时就pop
    for(auto idx:engSet){
        auto & word_pair = pdict->_dictEng[idx];
        /* if(pdict->_result.size() == pdict->_pqSize){ */
        /*     pdict->_result.pop(); */
        /* } */
        pdict->_result.emplace(word_pair.first,word_pair.second,editDistance(word_pair.first,key));
    }
    for(auto idx:zhSet){
        auto & word_pair = pdict->_dictZh[idx];
        /* if(pdict->_result.size() == pdict->_pqSize){ */
        /*     pdict->_result.pop(); */
        /* } */
        pdict->_result.emplace(word_pair.first,word_pair.second,editDistance(word_pair.first,key));
    }
    // 返回优先级队列的倒序内容(从大到小)
    /* vector<string> temp; */
    /* for(size_t i = 0; i < pdict->_result.size(); i++){ */
    /*     temp.emplace_back(pdict->_result.top()._word); */
    /*     pdict->_result.pop(); */
    /* } */
    /* return string(temp.rbegin(),temp.rend()); */
    string result;
    /* for(auto it = temp.rbegin(); it != temp.rend(); ++it){ */
    /*     result += *it; */
    /*     result += " "; */
    /* } */
    for(size_t i = 0; i < pdict->_pqSize && !pdict->_result.empty(); ++i){
        result += pdict->_result.top()._word + " ";
        pdict->_result.pop();
    }
    priority_queue<CandidateResult> temp;
    pdict->_result.swap(temp);
    return result + "\n";
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

