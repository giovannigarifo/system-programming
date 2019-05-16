//
// Created by giovanni on 13/05/19.
//

#ifndef LAB2_DIRECTORY_H
#define LAB2_DIRECTORY_H

#include <memory>
#include <vector>
#include "Base.h"
#include "File.h"
#include "FsException.h"

using namespace std;

/*
 * Singleton class
 */
class Directory: public Base{

private:
    // attributes
    weak_ptr<Directory> parentDir; //a pointer to the parent directory (if present)
    weak_ptr<Directory> selfDir; //a pointer to the directory itself
    vector<shared_ptr<Base>> dirContent; //container of shared_ptr to the content (Directories or Files) of the dir

    // singleton instance
    static shared_ptr<Directory> root;

    //support functions
    void setSelfDir(shared_ptr<Directory> selfPtr);

protected:
    // methods
    Directory(string dirName, weak_ptr<Directory> parentDir); //has private constructors
    Directory(Directory const& source); //has private copy constructor
    Directory& operator=(Directory const& source) = delete; //has private assignment operator


public:

    static shared_ptr<Directory> getRoot();
    shared_ptr<Directory> addDirectory(string dirName);
    shared_ptr<File> addFile(string name, uintmax_t size);
    shared_ptr<Base> get(string name);
    shared_ptr<Directory> getDir(string name);
    shared_ptr<File> getFile(string name);
    void remove(string name);

    // implement virtual functions
    int mType() const override;
    void ls(int indent) const override;
};


#endif //LAB2_DIRECTORY_H
