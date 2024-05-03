#ifndef _DIRSCANNER_H
#define _DIRSCANNER_H

#include <string>
#include <vector>
using std::string;
using std::vector;

class DirScanner {
public: 
    vector<string>& getFiles();
    void traverse(const string& dir);
private: 
    vector<string> _files;
};

inline vector<string>& DirScanner::getFiles() {
    return _files;
}

#endif //_DIRSCANNER_H
