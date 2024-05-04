#include <iostream>
#include <string>
#include "PageLib.h"
#include "../shared/Mylogger.h"
#include "../shared/Configuration.h"
#include "../shared/SplitTool.h"
#include "PageLibPreprocessor.h"
#include "DictProducer.h"
using namespace std;

void test_PageLib(){
    PageLib pl(Configuration::getInstance());
    pl.create();
    pl.store();
}

void test_Mylogger(){
    LogError("err msg");
    LogInfo("info msg");
}

void test_Configuration(){
    auto configs = Configuration::getConfigMap();
    for(auto & config : configs){
        cout << config.first << " " << config.second << "\n";
    }
}

void test_SplitTool(){
    SplitToolCppJieba Jieba(Configuration::getInstance());
    auto words = Jieba.cutWord("我是来卖萌的hello world！\n");
    for(auto &word:words){
        cout << word << "\n";
    }
}

void test_PageLibPreprocessor(){
    SplitToolCppJieba Jieba(Configuration::getInstance());
    PageLibPreprocessor plp{Configuration::getInstance(),&Jieba};
    plp.cutRedundantPage();
    plp.bulidInvertIndexMap();
    plp.storeOnDisk();
}

void test_DictProducer(){
    auto configs = Configuration::getConfigMap();
    SplitToolCppJieba Jieba(Configuration::getInstance());
    DictProducer dp(configs["dict_files_zh"],&Jieba);
    dp.buildDict();
}

int main()
{
    /* test_PageLib(); */
    /* test_Mylogger(); */
    /* test_Configuration(); */
    /* test_SplitTool(); */
    /* test_PageLibPreprocessor(); */
    test_DictProducer();

    return 0;
}

