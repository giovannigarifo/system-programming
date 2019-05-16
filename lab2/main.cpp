#include <iostream>

#include "Base.h"
#include "Directory.h"
#include "FsException.h"

int main() {

    try {

        cout<<"Testing lab exercise specifications are met:\n"<<endl;

        shared_ptr<Directory> root = Directory::getRoot();

        //test creation of already present dirname
        shared_ptr<Directory> dir1 = root->addDirectory("dir1");
        shared_ptr<Directory> dir2 = root->addDirectory("dir2");

        shared_ptr<Base> testget = root->get("dir1");

        shared_ptr<File> file1 = root->addFile("file1", 100);

        shared_ptr<Base> b1 = root->get("file1"); //get as pointer to Base

        shared_ptr<File> file2 = dir1->addFile("file2", 356);
        shared_ptr<File> file3 = dir1->addFile("file3", 356);
        shared_ptr<Directory> dir3 = dir1->addDirectory("dir3");

        root->ls(0);

        shared_ptr<File> file4 = root->addFile("file4", 256);
        dir1->remove("file3");

        root->ls(0);

        cout<<"Now testing exception handling: "<<endl;
        dir1 = root->addDirectory("dir1");

    }catch(FsException& fsExc){
        cerr<<fsExc.what();
    }

    return 0;
}