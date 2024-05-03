#include <regex>
#include <sstream>
#include "tinyxml2.h"
#include "Mylogger.h"
#include "FileProcessor.h"
using namespace tinyxml2;
using std::ostringstream;

// RSS文件每个item内容
struct RSSItem{
    RSSItem(const string& title,const string& link,const string& description)
        :_title(title),_link(link),_description(description)
    {}
    string _title;
    string _link;
    string _description;
};

vector<string> FileProcessor::process(const string& fileName) {
    vector<RSSItem> _items;
    _items.reserve(20);
    // 1 加载xml文件
    XMLDocument doc;
    doc.LoadFile(fileName.c_str());
    // 2 检查报错
    if(doc.ErrorID()){
        LogError("XML loadfile failed\n");
        return vector<string>();
    }
    // 3 处理
    // 3.1 获取rss-->channel-->第一个item
    XMLElement * pItemNode = doc.FirstChildElement("rss")
        ->FirstChildElement("channel")
        ->FirstChildElement("item");
    // 3.2 循环处理所有item
    while(pItemNode){
        string title,link,description;
        XMLElement * temp = pItemNode->FirstChildElement("description");
        // 跳过坏数据
        if(temp == nullptr){
            pItemNode = pItemNode->NextSiblingElement("item");
            continue;
        }
        description = temp->GetText();
        temp = pItemNode->FirstChildElement("title");
        if(temp != nullptr){
            title = temp->GetText();
        }
        temp = pItemNode->FirstChildElement("link");
        if(temp != nullptr){
            link = temp->GetText();
        }
        // 正则表达式处理content
        std::regex reg("<[^>]*>");
        description = regex_replace(description,reg,"");
        // 插入vector
        _items.emplace_back(title,link,description);
        // 获取下一个item结点
        pItemNode = pItemNode->NextSiblingElement("item");
    }
    // 4 输出
    vector<string> result;
    result.reserve(_items.size());
    for(size_t idx = 0; idx != _items.size(); ++idx){
        ostringstream oss;
        oss << "<doc>\n<title>" << _items[idx]._title
            << "</title>\n<link>" << _items[idx]._link
            << "</link>\n<content>" << _items[idx]._description 
            << "</content>\n</doc>\n";
        result.emplace_back(oss.str());
    }
    return result;
}
