//
// Created by giovanni.garifo@outlook.com on 26/04/19.
//

#include <cstring>
#include <cstdio>
#include <iostream>
#include "Buffer.h"

// constructor
Buffer::Buffer(int size) : size(size) {

    buf = new int[size];
}

// destructor
Buffer::~Buffer() {

    delete[] buf;
    buf = nullptr;
}

// copy constructor
Buffer::Buffer(const Buffer& source) {

    this->size = source.size;
    this->buf = new int[source.size];
    memcpy(this->buf, source.buf, source.size * sizeof(int));
}

// move contructor
Buffer::Buffer(Buffer &&source) {

    std::cout << "moving buffer!" << std::endl;
    this->size = source.size;
    this->buf = source.buf;
    source.buf = nullptr;
}

//operator "+" overloading
Buffer Buffer::operator + (Buffer other){

    Buffer sum = Buffer(this->size + other.size);

    memcpy(sum.buf, this->buf, this->size * sizeof(int));
    memcpy(sum.buf + this->size, other.buf, other.size * sizeof(int));

    return sum;
}

//
// Methods
//

//get the size of the buffer
int Buffer::getSize() {
    return this->size;
}

//get a value from the buffer and put it in val
bool Buffer::getData(int pos, int &val) {

    if(pos >= this->size)
        return false;

    memcpy(&val, this->buf + pos, sizeof(int));
    return true;
}

//set a value in the buffer
bool Buffer::setData(int pos, int &val) {

    if(pos >= this->size) return false;

    memcpy(this->buf + pos, &val, sizeof(int));
    return true;
}

