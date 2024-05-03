#ifndef _WEBPAGE_H
#define _WEBPAGE_H

#include <string>
#include <map>
using std::string;
using std::map;

class WebPage {
    friend class PageLibPreprocessor;
public:
    WebPage(const string& title,const string& link,
            const string& content,uint64_t simhash);
    string& getDoc();
private: 
    string _title;
    string _link;
    string _content;
    uint64_t _simhash;
};

#endif //_WEBPAGE_H
