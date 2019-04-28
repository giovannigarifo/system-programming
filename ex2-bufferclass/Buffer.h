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

    /*
     * the following six member functions are declared implicitly by the
     * C++ compiler if not explicitly defined by the programmer. In this
     * example they're explicitly implemented.
     */

    //constructor and destructor
    Buffer(int size);
    ~Buffer();

    //copy constructor and assignment operator
    Buffer(const Buffer& source);
    Buffer& operator = (const Buffer& source);

    //move contructor and move assignment operator
    Buffer(Buffer&& source);
    Buffer& operator = (Buffer&& source);


    /*
     * Additional member functions
     */

    Buffer operator + (Buffer& other);
    int getSize();
    bool getData(int pos, int& val);
    bool setData(int pos, int& val);
};


#endif //EX2_BUFFERCLASS_BUFFER_H
