#ifndef _APIGATEWAY_H
#define _APIGATEWAY_H

#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>
#include "service.srpc.h"
using wfrest::HttpReq;
using wfrest::HttpResp;

class APIGateway
{
public:
    APIGateway(int cnt)
    :_waitGroup(cnt)
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
    }
    ~APIGateway() {
        google::protobuf::ShutdownProtobufLibrary();
    }
    void start(unsigned short port);
    void loadModules(){
        loadStaticResourceModule();
        loadKeyWordModule();
        loadWebQueryModule();
    }
private:
    void loadStaticResourceModule();
    // 关键词推荐模块
    void loadKeyWordModule();
    void KeyWordCb(const HttpReq *req, HttpResp * resp, SeriesWork * series);
    void KeyWordSrpc(const HttpReq *req, HttpResp * resp, WFHttpTask *httpTask);
    void keyword_done(HttpResp * resp, respInfo *response, srpc::RPCContext *context);
    // 网页查询模块
    void loadWebQueryModule();
    void WebQueryCb(const HttpReq *req, HttpResp * resp, SeriesWork * series);
    void WebQuerySrpc(const HttpReq *req, HttpResp * resp, WFHttpTask *httpTask);
    void webquery_done(HttpResp * resp, respInfo *response, srpc::RPCContext *context);
private:
    WFFacilities::WaitGroup _waitGroup;
    wfrest::HttpServer _httpserver;
};

#endif
