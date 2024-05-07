#include "APIGateway.h"

int main()
{
    // 创建服务器
    APIGateway apiGateway(1);
    // 加载模块
    apiGateway.loadModules();
    // 启动服务器
    apiGateway.start(1234);
    return 0;
}

