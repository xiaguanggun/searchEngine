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
    cout << con->toString() << " is connecting!\n";
}

/**
 * @param con
 * @return void
 */
void Reactor::onMessage(TcpConnectionPtr con) {
    string msg = con->recv();
    cout << "recv msg = " << msg << "\n";
    if(msg == "WebQuery"){
        _pool.addTask(shared_ptr<Task>(new WebQueryTask(msg,con)));
    }
    else if (msg == "KeyWordTask"){
        _pool.addTask(shared_ptr<Task>(new KeyWordTask(msg,con)));
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
    cout << con->toString() << " is closed!!!\n";
}

