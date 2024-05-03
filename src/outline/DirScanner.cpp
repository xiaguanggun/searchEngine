#include <dirent.h>

#include "Mylogger.h"
#include "DirScanner.h"

void DirScanner::traverse(const string& dir) {
    DIR *directory;
    struct dirent *ent;
    if ((directory = ::opendir(dir.c_str())) != nullptr) {
        while ((ent = ::readdir(directory)) != nullptr) {
            if (ent->d_type == DT_REG) { // Regular file
                _files.push_back(dir + "/" + ent->d_name);
            }
        }
        ::closedir(directory);
    } else {
        LogError("Error opening directory:"+dir);
    }
}

