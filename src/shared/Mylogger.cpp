#include "Mylogger.h"
#include <iostream>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

using namespace log4cpp;

#define LOGSIZE 10*1024
#define RFLOGNUM 3
#define LOGFORM "%d %c [%p] %m%n"

// static members
Mylogger * Mylogger::_pInstance = nullptr;
Mylogger * Mylogger::getInstance(){
    if(!_pInstance){
        _pInstance = new Mylogger();
    }
    return _pInstance;
}
void Mylogger::destory(){
    if(_pInstance){
        delete _pInstance;
        _pInstance = nullptr;
    }
}

// 构造、析构
Mylogger::Mylogger()
:_mycat(Category::getInstance("mycat"))
{
    // 自动销毁
    ::atexit(&Mylogger::destory);
    // 记录器 + 过滤器
    _mycat.setPriority(Priority::DEBUG);
    // 输出器 + 格式化器
    OstreamAppender * osAppender = new OstreamAppender("console",&std::cout);
    PatternLayout * osPtn = new PatternLayout();
    osPtn->setConversionPattern(LOGFORM);
    osAppender->setLayout(osPtn);
    /* FileAppender * fAppender = new FileAppender("fileApp","../log/file.log"); */
    /* PatternLayout * fPtn = new PatternLayout(); */
    /* fPtn->setConversionPattern(LOGFORM); */
    /* fAppender->setLayout(fPtn); */
    RollingFileAppender * rfAppender = new RollingFileAppender("rfApp","../log/rf.log",LOGSIZE,RFLOGNUM);
    PatternLayout * rfPtn = new PatternLayout();
    rfPtn->setConversionPattern(LOGFORM);
    rfAppender->setLayout(rfPtn);
    // 连接 记录器 输出器
    _mycat.addAppender(osAppender);
    /* _mycat.addAppender(fAppender); */
    _mycat.addAppender(rfAppender);
}
Mylogger::~Mylogger(){
    Category::shutdown();
}

// 成员函数
void Mylogger::error(const char *msg){
    _mycat.error(msg);
}
void Mylogger::warn(const char *msg){
    _mycat.warn(msg);
}
void Mylogger::info(const char *msg){
    _mycat.info(msg);
}
void Mylogger::debug(const char *msg){
    _mycat.debug(msg);
}

