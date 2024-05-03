#include "WebPage.h"

WebPage::WebPage(const string& title,const string& link,
                 const string& content,uint64_t simhash)
:_title(title)
,_link(link)
,_content(content)
,_simhash(simhash)
{}

string& WebPage::getDoc() {
    return _content;
}
