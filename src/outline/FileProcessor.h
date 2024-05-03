#ifndef _FILEPROCESSOR_H
#define _FILEPROCESSOR_H

#include <vector>
#include <string>
using std::vector;
using std::string;

class FileProcessor {
public: 
    vector<string> process(const string& fileName);
};

#endif //_FILEPROCESSOR_H
