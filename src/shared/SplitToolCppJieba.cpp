#include "../../include/cppjieba/Jieba.hpp"
#include "Configuration.h"
#include "SplitTool.h"

// 内部类
class SplitToolCppJieba::CppJiebaImpl{
public:
    CppJiebaImpl(Configuration * pconf);
    ~CppJiebaImpl();
    vector<string> cutAll(const string& sentence);
    vector<string> cutWord(const string& sentence);
private:
    cppjieba::Jieba * _pjieba = nullptr;
};

// 外部类函数实现
SplitToolCppJieba::SplitToolCppJieba(Configuration * pconf)
:_pimpl(new CppJiebaImpl(pconf))
{}
SplitToolCppJieba::~SplitToolCppJieba(){
    if(_pimpl != nullptr){
        delete _pimpl;
        _pimpl = nullptr;
    }
}
vector<string> SplitToolCppJieba::cutAll(const string& sentence) {
    return _pimpl->cutAll(sentence);
}
vector<string> SplitToolCppJieba::cutWord(const string& sentence) {
    return _pimpl->cutWord(sentence);
}

// 内部类函数实现
SplitToolCppJieba::CppJiebaImpl::CppJiebaImpl(Configuration * pconf){
    // 获取字典
    auto configs = pconf->getConfigMap();
    const char* const DICT_PATH = configs["DICT_PATH"].c_str();
    const char* const HMM_PATH = configs["HMM_PATH"].c_str();
    const char* const USER_DICT_PATH = configs["USER_DICT_PATH"].c_str();
    /* const char* const IDF_PATH = configs["IDF_PATH"].c_str(); */
    /* const char* const STOP_WORD_PATH = configs["STOP_WORD_PATH"].c_str(); */
    // 创建jieba对象
    _pjieba = new cppjieba::Jieba{DICT_PATH,HMM_PATH,USER_DICT_PATH};
}
SplitToolCppJieba::CppJiebaImpl::~CppJiebaImpl(){
    if(_pjieba != nullptr){
        delete _pjieba;
        _pjieba = nullptr;
    }
}
vector<string> SplitToolCppJieba::CppJiebaImpl::cutAll(const string& sentence){
    vector<string> words;
    //
    return words;
}
vector<string> SplitToolCppJieba::CppJiebaImpl::cutWord(const string& sentence){
    vector<string> words;
    _pjieba->Cut(sentence,words,true);
    return words;
}

