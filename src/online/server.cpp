#include <iostream>
#include <fstream>
#include "../reactor/macro.h"
#include "../reactor/Reactor.h"

#include "../shared/Configuration.h"
#include "../shared/Mylogger.h"
#include "Dictionary.h"
#include "WebPageQuery.h"

using namespace std;

int main()
{
    // 加载配置文件
    auto configs = Configuration::getConfigMap();
    auto stopWords = Configuration::getStopWords();
    // 获取服务端配置文件
    string server_conf = configs["server.conf"];
    ifstream ifs(server_conf);
    if(!ifs){
        LogError("open server.conf failed");
        return -1;
    }
    size_t threadNum, qSize;
    string ip;
    unsigned short port;
    int timeout = 3000;
    ifs >> threadNum >> qSize >> ip >> port;
    if(ifs){
        ifs >> timeout;
    }
    /* cout << threadNum << qSize << ip << port << timeout << "\n"; */
    ifs.close();
    // 初始化dict & webpage
    Dictionary::init(Configuration::getInstance());
    WebPageQuery::init(Configuration::getInstance());
    // 启动服务器
    Reactor server(threadNum,qSize,ip,port,timeout);
    server.start();

    cout << "Reactor closed\n";
    return 0;
}

