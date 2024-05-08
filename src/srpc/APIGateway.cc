#include <string>
#include <workflow/RouteManager.h>
#include <ppconsul/consul.h>
#include <wfrest/json.hpp>
#include "info.h"
#include "APIGateway.h"
using namespace std::placeholders;
using namespace wfrest;
using std::string;
using std::cout;

void APIGateway::start(unsigned short port)
{
    if(_httpserver.track().start(port) == 0) {
        _httpserver.list_routes();
        _waitGroup.wait();
        _httpserver.stop();
    } else {
        cout << "APIGateway Start Failed!\n";
    }
}

void APIGateway::loadStaticResourceModule(){
    _httpserver.GET("/", [](const HttpReq *, HttpResp * resp){
                    resp->File("../data/home.html");
                    });
}

void APIGateway::loadKeyWordModule()
{
    _httpserver.POST("/KeyWord", std::bind(&APIGateway::KeyWordCb,this,_1,_2,_3));
}
void APIGateway::KeyWordCb(const HttpReq *req, HttpResp * resp, SeriesWork * series){
    // 获取所有InfoService ip&port返回值是json文件
    WFHttpTask * httpTask = WFTaskFactory::create_http_task
        (string("http://")+CONSULIPPORT+"/v1/health/service/"+KEYWORDSERVICENAME,
         10,0,std::bind(&APIGateway::KeyWordSrpc,this,req,resp,_1));
    series->push_back(httpTask);
}
void APIGateway::KeyWordSrpc(const HttpReq *req, HttpResp * resp, WFHttpTask *httpTask){
    if(req->content_type() != APPLICATION_JSON) {
        resp->String("Content_type error");
        return;
    }
    // 1.获取rpc的ip&port,解析json文件
    const void *body;
    size_t size;
    httpTask->get_resp()->get_parsed_body(&body,&size);
    nlohmann::json jsons = nlohmann::json::parse((const char *)body);                                                                                  
    string ip;
    unsigned short port = 0000;
    // 2.遍历所有结点
    for(auto & json:jsons){
        if(json["Checks"][1]["Status"] == "passing"){
            ip = json["Service"]["Address"];
            port = json["Service"]["Port"];
            cout << KEYWORDSERVICENAME" is ok , ip = " << ip << " port = " << port << "\n";
            break;
        }
    }
    if(ip.size() == 0){
        cout << KEYWORDSERVICENAME" is closed!\n";
        resp->String(KEYWORDSERVICENAME" is closed!");
        return;
    }
    // 3.获取key
    /* Form &form = req->form(); */
    /* if (form.empty()){ */
    /*     resp->set_status(HttpStatusBadRequest); */
    /*     return; */
    /* } */ 
    /* string key = form.begin()->second.second; */
    nlohmann::json json = nlohmann::json::parse(req->body());
    string key = json["keyword"];
    cout << key << "\n";
    // 4.rpc调用
	KeyWord::SRPCClient client(ip.c_str(), port);
	reqInfo keyword_req;
    keyword_req.set_key(key);
    srpc::SRPCClientTask *KeyWordTask =
        client.create_KeyWord_task(std::bind(&APIGateway::keyword_done,this,resp,_1,_2));
    KeyWordTask->serialize_input(&keyword_req);
    series_of(httpTask)->push_back(KeyWordTask);
}
void APIGateway::keyword_done(HttpResp * resp, respInfo *response, srpc::RPCContext *context){
    //1.获取rpc响应内容
    string msg = response->msg();
    //2.回复客户端
    /* cout << msg << "\n"; */
    /* resp->set_header_pair("Content-Type","application/json"); */
    /* resp->String(msg); */
    nlohmann::json json = nlohmann::json::parse(msg);
    resp->Json(json);
}

// 网页查询模块
void APIGateway::loadWebQueryModule()
{
    _httpserver.POST("/WebQuery", std::bind(&APIGateway::WebQueryCb,this,_1,_2,_3));
}
void APIGateway::WebQueryCb(const HttpReq *req, HttpResp * resp, SeriesWork * series){
    // 获取所有InfoService ip&port返回值是json文件
    WFHttpTask * httpTask = WFTaskFactory::create_http_task
        (string("http://")+CONSULIPPORT+"/v1/health/service/"+WEBQUERYSERVICENAME,
         10,0,std::bind(&APIGateway::WebQuerySrpc,this,req,resp,_1));
    series->push_back(httpTask);
}
void APIGateway::WebQuerySrpc(const HttpReq *req, HttpResp * resp, WFHttpTask *httpTask){
    if(req->content_type() != APPLICATION_JSON) {
        resp->String("Content_type error");
        return;
    }
    // 1.获取rpc的ip&port,解析json文件
    const void *body;
    size_t size;
    httpTask->get_resp()->get_parsed_body(&body,&size);
    nlohmann::json jsons = nlohmann::json::parse((const char *)body);                                                                                  
    string ip;
    unsigned short port = 0000;
    // 2.遍历所有结点
    for(auto & json:jsons){
        if(json["Checks"][1]["Status"] == "passing"){
            ip = json["Service"]["Address"];
            port = json["Service"]["Port"];
            cout << WEBQUERYSERVICENAME" is ok , ip = " << ip << " port = " << port << "\n";
            break;
        }
    }
    if(ip.size() == 0){
        cout << WEBQUERYSERVICENAME" is closed!\n";
        resp->String(WEBQUERYSERVICENAME" is closed!");
        return;
    }
    // 3.获取用户名和密码
    /* Form &form = req->form(); */
    /* if (form.empty()){ */
    /*     resp->set_status(HttpStatusBadRequest); */
    /*     return; */
    /* } */ 
    /* string key = form.begin()->second.second; */
    nlohmann::json json = nlohmann::json::parse(req->body());
    string key = json["keyword"];
    cout << key << "\n";
    // 4.rpc调用
	WebQuery::SRPCClient client(ip.c_str(), port);
	reqInfo WebQuery_req;
    WebQuery_req.set_key(key);
    srpc::SRPCClientTask *WebQueryTask =
        client.create_WebQuery_task(std::bind(&APIGateway::webquery_done,this,resp,_1,_2));
    WebQueryTask->serialize_input(&WebQuery_req);
    series_of(httpTask)->push_back(WebQueryTask);
}
void APIGateway::webquery_done(HttpResp * resp, respInfo *response, srpc::RPCContext *context){
    //1.获取rpc响应内容
    string msg = response->msg();
    //2.回复客户端
    /* cout << msg << "\n"; */
    /* resp->set_header_pair("Content-Type","application/json"); */
    /* resp->String(msg); */
    nlohmann::json json = nlohmann::json::parse(msg);
    resp->Json(json);
}
