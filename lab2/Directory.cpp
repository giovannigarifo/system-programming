//
// Created by giovanni on 13/05/19.
//

#include <iostream>
#include <utility>
#include "Directory.h"

//explicitly initialize the static variable to NULL
shared_ptr<Directory> Directory::root = NULL;


/****************
 * Constructors *
 ****************/
Directory::Directory(string dirName, weak_ptr<Directory> parentDir){
    this->setName(std::move(dirName));
    this-> parentDir = std::move(parentDir);
    this-> selfDir = weak_ptr<Directory>();
    this-> dirContent = vector<shared_ptr<Base>>();
}


/***********************************
 * Member functions implementation *
 ***********************************/


/*
 * Given a shared_ptr to the Directory object itself, creates the selfDir weak_ptr
 */
void Directory::setSelfDir(shared_ptr<Directory> selfPtr){
    this->selfDir = weak_ptr<Directory>(selfPtr);
}

/*
 * Creates, if doesn't already exist, the root directory and return a shared pointer to it.
 * If it exists, just return the pointer to the root directory.
 */
shared_ptr<Directory> Directory::getRoot() {

    //if doesn't exist instantiate the root dir.
    if (!root) {
        root = shared_ptr<Directory>(new Directory("/", weak_ptr<Directory>())); //initially selfDir is nullptr
        root->setSelfDir(root);
    }

    return root;
}

/*
 * Create a new Directory object, given the name, and add it to the current folder.
 * If the name is already used, throw an exception.
 */
shared_ptr<Directory> Directory::addDirectory(string dirName){

    //test if dirName is already present in current director
    for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){

        shared_ptr<Base> sp = *it;

        if(sp->getName() == dirName)
            throw FsException();
    }

    //create new dir object with name and parent
    Directory* newDir = new Directory(dirName, this->selfDir);

    //create shared pointer to the new directory
    shared_ptr<Directory> newDirSharedPtr = shared_ptr<Directory>(newDir);
    newDir->setSelfDir(newDirSharedPtr);

    //add directory to current dirContent
    this->dirContent.push_back(newDirSharedPtr);

    return newDirSharedPtr;
}

/*
 * Get the pointer to the Directory or File with the given name in the current folder.
 * If not exist, return an empty shared_ptr.
 */
shared_ptr<Base> Directory::get(string name) {

    if(name == "."){

        if(shared_ptr<Base> sp =  this->selfDir.lock())
            return sp;

    } else if (name == "..") {

        if (shared_ptr<Base> sp = this->parentDir.lock())
            return sp;

    } else {

        for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){
            shared_ptr<Base> sp = *it;
            if(sp->getName() == name) return sp;
        }
    }

    return shared_ptr<Base>();
}

/*
 * Same as get() but return a shared_ptr<Directory>
 */
shared_ptr<Directory> Directory::getDir(string name) {

    if(name == "."){

        if(shared_ptr<Base> sp =  this->selfDir.lock())
            return dynamic_pointer_cast<Directory>(sp);

    } else if (name == "..") {

        if (shared_ptr<Base> sp = this->parentDir.lock())
            return dynamic_pointer_cast<Directory>(sp);

    } else {

        for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){

            shared_ptr<Base> spb = *it;
            shared_ptr<Directory> sp = dynamic_pointer_cast<Directory>(spb);

            if(sp != nullptr)
                if(sp->getName() == name)
                    return sp;
        }
    }

    return shared_ptr<Directory>();
}

/*
 * Remove Directory or File with given name from the current Directory.
 * If not present throws an exception.
 */
void Directory::remove(string name) {

    if(name == "." || name == "..")
        throw FsException();
    else {

        for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){

            shared_ptr<Base> sp = *it;

            if(sp->getName() == name){
                this->dirContent.erase(it); //also automagically destroys the shared_ptr
            }
        }
    }

}

/*
 * add a file to the directory
 */
shared_ptr<File> Directory::addFile(string name, uintmax_t size) {

    //test if a file or directory with name is already present in current director
    for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){

        shared_ptr<Base> dirPtr = (*it);

        if(dirPtr->getName() == name)
            throw FsException();
    }

    //create new File object with name and parent
    File* newFile = new File(name, size);

    //create shared pointer to the new file
    shared_ptr<File> newFileSharedPtr = shared_ptr<File>(newFile);

    //add File to current dirContent
    this->dirContent.push_back(newFileSharedPtr);

    return newFileSharedPtr;
}

/*
 * Same as get() but returns a File
 */
shared_ptr<File> Directory::getFile(string name) {

    for(auto it = this->dirContent.begin(); it < this->dirContent.end(); it++){

        shared_ptr<Base> spb = *it;
        shared_ptr<File> sp = dynamic_pointer_cast<File>(spb);

        if(sp != nullptr)
            if(sp->getName() == name)
                return sp;
    }

    return shared_ptr<File>();
}


int Directory::mType() const {
    return 0; // 0 == Directory Type
}

/*
 * List recursively the content of the directory and subdirectories
 */
void Directory::ls(int indent) const {

    if(this->dirContent.empty()){

        //indentation
        string tabs;
        for(int i=0; i<indent;i++)
            tabs.append("\t");

        //print current dir
        cout<<tabs<<"[+] "<<this->getName()<<endl;

        return;

    }else{

        //indentation
        string tabs;
        for(int i=0; i<indent;i++)
            tabs.append("\t");

        //print current dir
        cout<<tabs<<"[+] "<<this->getName()<<endl;

        //call ls on contents
        for(auto it = dirContent.begin(); it<dirContent.end(); it++){
            shared_ptr<Base> spb = *it;

            shared_ptr<Directory> spd = dynamic_pointer_cast<Directory>(spb);
            if(spd != nullptr)
                spd->ls(indent+1);
            else{
                shared_ptr<File> spf = dynamic_pointer_cast<File>(spb);
                spf->ls(indent+1);
            }
        }
    }

    if(indent==0)
        cout<<endl;
}

