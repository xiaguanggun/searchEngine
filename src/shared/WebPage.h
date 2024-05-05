#ifndef _WEBPAGE_H
#define _WEBPAGE_H

#include <string>
#include <map>
using std::string;
using std::map;

class WebPage {
    friend class PageLibPreprocessor;
    friend class WebPageQuery;
public:
    WebPage(const string& title,
            const string& link,
            const string& content);
    string& getDoc();
private: 
    string _title;
    string _link;
    string _content;
};

#endif //_WEBPAGE_H
