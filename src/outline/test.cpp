#include <iostream>
#include <string>
#include "PageLib.h"
#include "Mylogger.h"
#include "Configuration.h"
#include "SplitTool.h"
#include "PageLibPreprocessor.h"
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

int main()
{
    /* test_PageLib(); */
    /* test_Mylogger(); */
    /* test_Configuration(); */
    /* test_SplitTool(); */
    test_PageLibPreprocessor();

    return 0;
}

