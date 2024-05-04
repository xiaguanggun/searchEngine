#ifndef __Mylogger_H__
#define __Mylogger_H__

#include <log4cpp/Category.hh>

#define addPrefix(msg) string("[").append(__FILE__).append(":")\
                                  .append(__func__).append(":")\
                                  .append(std::to_string(__LINE__))\
                                  .append("]  ").append(msg).c_str()
#define LogError(msg) Mylogger::getInstance()->error(addPrefix(msg));
#define LogWarn(msg) Mylogger::getInstance()->warn(addPrefix(msg));
#define LogInfo(msg) Mylogger::getInstance()->info(addPrefix(msg));
#define LogDebug(msg) Mylogger::getInstance()->debug(addPrefix(msg));

class Mylogger
{
public:
	void error(const char *msg);
	void warn(const char *msg);
	void info(const char *msg);
	void debug(const char *msg);
	
    static Mylogger * getInstance();
private:
	Mylogger();
	~Mylogger();
    static void destory();
private:
    log4cpp::Category & _mycat;
    static Mylogger * _pInstance;
};

#endif
