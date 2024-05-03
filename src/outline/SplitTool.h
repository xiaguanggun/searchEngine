#ifndef _SPLITTOOL_H
#define _SPLITTOOL_H

#include <vector>
#include <string>
using std::vector;
using std::string;

class SplitTool {
public: 
    virtual ~SplitTool(){};
    virtual vector<string> cutAll(const string& sentence) = 0;
    virtual vector<string> cutWord(const string& sentence) = 0;
};

class Configuration;
class SplitToolCppJieba: public SplitTool {
public: 
    SplitToolCppJieba(Configuration * pconf);
    ~SplitToolCppJieba() override;
    vector<string> cutAll(const string& sentence) override;
    vector<string> cutWord(const string& sentence) override;
private:
    class CppJiebaImpl;
    CppJiebaImpl * _pimpl;
};

#endif //_SPLITTOOL_H
