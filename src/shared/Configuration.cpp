#include "Configuration.h"

// 静态变量
string Configuration::_confFilePath = "../conf/myconf.conf";
Configuration * Configuration::_pInstance = nullptr;
// 构造函数
Configuration::Configuration(){
    // 自动销毁
    ::atexit(&Configuration::destory);
    // 读取配置文件
    ifstream ifs(_confFilePath);
    if(!ifs){
        LogError("open conf_file failed\n");
        return;
    }
    string key,value;
    while(ifs){
        ifs >> key >> value;
        _configs[key] = value;
    }
    ifs.close();
    // 读取停用词
    ifs.open(_configs["stop_words_zh"]);
    if(!ifs){
        LogError("open stop_words_zh failed\n");
        return;
    }
    while(ifs){
        ifs >> key;
        _stopWords.emplace(key);
    }
    ifs.close();
    ifs.open(_configs["stop_words_eng"]);
    if(!ifs){
        LogError("open stop_words_eng failed\n");
        return;
    }
    while(ifs){
        ifs >> key;
        _stopWords.emplace(key);
    }
    ifs.close();
    // 停用词加入英文空格
    _stopWords.emplace(" ");
}

