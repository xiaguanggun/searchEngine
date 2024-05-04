#ifndef _SPLITTOOL_H
#define _SPLITTOOL_H

#include <vector>
#include <string>
using std::vector;
using std::string;

class SplitTool {
public: 
    virtual ~SplitTool(){};
    virtual vector<string> cutWord(const string& sentence) = 0;
};

class Configuration;
class SplitToolCppJieba: public SplitTool {
public: 
    SplitToolCppJieba(Configuration * pconf);
    ~SplitToolCppJieba() override;
    vector<string> cutWord(const string& sentence) override;
private:
    class CppJiebaImpl;
    CppJiebaImpl * _pimpl;
};

class SplitToolChar: public SplitTool{
public:
    vector<string> cutWord(const string& sentence) override;
};

#endif //_SPLITTOOL_H
