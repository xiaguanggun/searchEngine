/**
 * Project Reactor
 */


#include "Reactor.h"
#include "Task.h"

/**
 * Reactor implementation
 */


/**
 * @param ip
 * @param port
 * @param timeout
 * @param threadNum
 * @param qSize
 */
Reactor::Reactor(size_t threadNum, size_t qSize, const string& ip, unsigned short port, int timeout)
:_pool(threadNum,qSize)
,_server(ip,port,timeout)
{}

/**
 * @param ip
 * @param port
 * @param timeout
 * @param threadNum
 * @param qSize
 */
Reactor::Reactor(size_t threadNum, size_t qSize, const string& ip, const string& port, int timeout)
:_pool(threadNum,qSize)
,_server(ip,port,timeout)
{}

Reactor::~Reactor() {
    _pool.stop();
}

/**
 * @return void
 */
void Reactor::start() {
    _pool.start();
    using namespace std::placeholders;
    _server.setAllCallback(std::bind(&Reactor::onNewConn,this,_1),
                           std::bind(&Reactor::onMessage,this,_1),
                           std::bind(&Reactor::onClose,this,_1));
    _server.start();
}

/**
 * @return void
 */
void Reactor::stop() {
    _server.stop();
}

/**
 * @param con
 * @return void
 */
void Reactor::onNewConn(TcpConnectionPtr con) {
    /* cout << con->toString() << " is connecting!\n"; */
    LogInfo(con->toString() + " is connecting!");
}

/**
 * @param con
 * @return void
 */
void Reactor::onMessage(TcpConnectionPtr con) {
    string msg = con->recv();
    /* cout << "recv cmd = " << msg << "\n"; */
    if(msg.size() && msg[0] == '1'){
        /* msg = con->recv(); */
        // 去除末尾的换行
        if(msg.back() == '\n'){
            msg.resize(msg.size()-1);
        }
        LogInfo("recv cmd = KeyWord, msg = " + msg);
        _pool.addTask(shared_ptr<Task>(new KeyWordTask(msg,con)));
    }
    else if(msg.size() && msg[0] == '2'){
        /* msg = con->recv(); */
        // 去除末尾的换行
        if(msg.back() == '\n'){
            msg.resize(msg.size()-1);
        }
        LogInfo("recv cmd = WebQuery, msg = " + msg);
        _pool.addTask(shared_ptr<Task>(new WebQueryTask(msg,con)));
    }
    else{
        _pool.addTask(shared_ptr<Task>(new Task("Task Msg Error!",con)));
    }
}

/**
 * @param con
 * @return void
 */
void Reactor::onClose(TcpConnectionPtr con) {
    /* cout << con->toString() << " is closed!!!\n"; */
    LogInfo(con->toString() + " is closed!!!");
}

