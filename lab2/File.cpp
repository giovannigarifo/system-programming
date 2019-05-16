//
// Created by giovanni on 15/05/19.
//

#include "File.h"
#include <iostream>


/****************
 * Constructors *
 ****************/
File::File(string name, uintmax_t size){
    this->setName(name);
    this->size = size;
}


/***********************************
 * Member functions implementation *
 ***********************************/
uintmax_t File::getSize() const {
    return this->size;
}


/*
 * Print the file name and size according to the indentation needed
 */
void File::ls(int indent) const {

    //indentation
    string tabs;
    for(int i=0; i<indent;i++)
        tabs.append("\t");

    //print file info
    cout<<tabs<<this->getName()<<" "<<this->getSize()<<endl;

}

int File::mType() const {
    return 1; // 1 == File Type
}

