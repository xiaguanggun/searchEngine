#include "WebPage.h"

WebPage::WebPage(const string& title,
                 const string& link,
                 const string& content)
:_title(title)
,_link(link)
,_content(content)
{}

string& WebPage::getDoc() {
    return _content;
}
