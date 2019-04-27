//
// Created by giovanni.garifo@outlook.com on 26/04/19.
//

#ifndef EX2_BUFFERCLASS_BUFFER_H
#define EX2_BUFFERCLASS_BUFFER_H


#include <cstdint>

class Buffer {

private:
    int size;
    int* buf;


public:
    //constructor
    Buffer(int size);

    //destructor
    ~Buffer();

    //copy constructor
    Buffer(const Buffer &source);

    //move contructor
    Buffer(Buffer &&source);

    //operator overloading
    Buffer operator + (Buffer other);

    //methods
    int getSize();
    bool getData(int pos, int& val);
    bool setData(int pos, int& val);
};


#endif //EX2_BUFFERCLASS_BUFFER_H
