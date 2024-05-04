#include "Configuration.h"
#include "SplitTool.h"

// 获取下一个字符的字节数
size_t nBytesCode(const char ch){
    if(ch & (1 << 7)){
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx){
            if(ch & (1 << (6 - idx))){
                ++nBytes;	
            }
            else{
                break;
            }
        }
        return nBytes;
    }
    return 1;
}  

vector<string> SplitToolChar::cutWord(const string& sentence){
    vector<string> chars;
	for(std::size_t idx = 0; idx != sentence.size();){
		size_t nBytes = nBytesCode(sentence[idx]);
        chars.emplace_back(sentence.substr(idx,nBytes));
		idx += nBytes;
	}
    return chars;
}

