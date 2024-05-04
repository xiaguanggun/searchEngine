#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <string>
#include <map>
#include <unordered_set>
#include <fstream>
#include "Mylogger.h"
using std::string;
using std::map;
using std::unordered_set;
using std::ifstream;

// 单例
class Configuration {
private:
    Configuration();
public:
    static void setConfFilePath(const string& filename);
    static Configuration * getInstance();
    static void destory();
    static map<string,string>& getConfigMap();
    static unordered_set<string>& getStopWords();
private:
    static Configuration * _pInstance;
    static string _confFilePath;
    map<string,string> _configs;
    unordered_set<string> _stopWords;
};

inline void Configuration::setConfFilePath(const string& filename){
    _confFilePath = filename;
}

inline Configuration * Configuration::getInstance(){
    if(!_pInstance){
        _pInstance = new Configuration();
    }
    return _pInstance;
}
inline void Configuration::destory(){
    if(_pInstance){
        delete _pInstance;
        _pInstance = nullptr;
    }
}

inline map<string,string>& Configuration::getConfigMap(){
    return getInstance()->_configs;
}
inline unordered_set<string>& Configuration::getStopWords(){
    return getInstance()->_stopWords;
}

#endif //_CONFIGURATION_H
