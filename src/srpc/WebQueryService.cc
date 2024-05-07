#include <signal.h>
#include <iostream>
#include <fstream>
#include <ppconsul/ppconsul.h>
#include <workflow/WFFacilities.h>
#include "info.h"
#include "service.srpc.h"
#include "CacheManager.h"
#include "../shared/Mylogger.h"
#include "../online/WebPageQuery.h"
using namespace std::placeholders;
using namespace ppconsul;
using namespace srpc;
using std::cout;
using std::string;

#define SERVICEID WEBQUERYSERVICENAME"1"
#define CACHENUM 4
#define CACHEFILENAME "cachesWebQuery.dat"

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    LogInfo(WEBQUERYSERVICENAME" close");
	wait_group.done();
}

class WebQueryServiceImpl : public WebQuery::Service
{
public:
	void WebQuery(reqInfo *request, respInfo *response, srpc::RPCContext *ctx) override
	{
        size_t _threadId = std::hash<std::thread::id>{}(std::this_thread::get_id()) % CACHENUM + 1;
        cout << "threadId = " << _threadId << "\n";
        string key = request->key();
        LogInfo("WebQuery:" + key);
        string msg = CacheManager::queryCache(_threadId,key);
        if(msg.size() == 0){
            msg = WebPageQuery::doQuery(key);
            CacheManager::addCache(_threadId,key,msg);
        }
        response->set_msg(msg);
	}
};

// timerCb1 to servicePass
void timerCb1(WFTimerTask * timerTask){
    /* cout << "now = " << time(nullptr) << "\n"; */
    agent::Agent * pagent = (agent::Agent *)timerTask->user_data;
    pagent->servicePass(SERVICEID);
    WFTimerTask * nextTask =
        WFTaskFactory::create_timer_task(9,0,timerCb1);
    nextTask->user_data = pagent;
    series_of(timerTask)->push_back(nextTask);
}
// timerCb2 to updateCache
void timerCb2(WFTimerTask * timerTask){
    /* cout << "now = " << time(nullptr) << "\n"; */
    CacheManager::updateCache();
    WFTimerTask * nextTask =
        WFTaskFactory::create_timer_task(30,0,timerCb2);
    series_of(timerTask)->push_back(nextTask);
}

int main()
{
    /* ::signal(SIGINT,sig_handler); */
    // 初始化
    Configuration::getInstance();
    CacheManager::init(CACHENUM,CACHEFILENAME);
    WebPageQuery::init();

    // consul
    Consul consul(CONSULIPPORT,kw::dc = CONSULDC);
    agent::Agent agent(consul);
    // 注册服务
    agent.registerService(
                          agent::kw::name = WEBQUERYSERVICENAME,
                          agent::kw::address = WEBQUERYSERVICEIP,
                          agent::kw::id = SERVICEID,
                          agent::kw::port = WEBQUERYSERVICEPORT,
                          agent::kw::check = agent::TtlCheck(std::chrono::seconds(10))
                         );
    // 发送心跳
    agent.servicePass(SERVICEID);
    // timerTask to servicePass
    WFTimerTask * timerTask1 =
        WFTaskFactory::create_timer_task(9,0,timerCb1);
    timerTask1->user_data = &agent;
    timerTask1->start();
    // timerTask to updateCache
    WFTimerTask * timerTask2 =
        WFTaskFactory::create_timer_task(30,0,timerCb2);
    timerTask2->start();

    // SRPCServer
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	unsigned short port = WEBQUERYSERVICEPORT;
	SRPCServer server;

	WebQueryServiceImpl webquery_impl;
	server.add_service(&webquery_impl);

	server.start(port);
	wait_group.wait();
    CacheManager::writeToFile();
    CacheManager::destory();
	server.stop();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
