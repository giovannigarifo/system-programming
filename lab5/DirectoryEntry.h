//
// Created by giovanni on 01/07/19.
//

#ifndef LAB5_DIRECTORYENTRY_H
#define LAB5_DIRECTORYENTRY_H

#include <string>

using namespace std;

class DirectoryEntry {

public:
    DirectoryEntry(const string &name, float size, bool isDir);
    DirectoryEntry();


public:
    bool isDir;
    string name;
    float size;
};


#endif //LAB5_DIRECTORYENTRY_H
