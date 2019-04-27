#include <iostream>
#include "Buffer.h"


//used just to test the move constructor
Buffer giveMeABuffer(int size);

int main() {

    std::cout << "\nHello, let's create some buffers!\n" << std::endl;

    Buffer myStaticallyAllocatedBuffer(5);
    Buffer* myDinamicallyAllocatedBuffer = new Buffer(10);

    std::cout << "size of static buffer: " << myStaticallyAllocatedBuffer.getSize() << std::endl;
    std::cout << "size of dynamic buffer: " << myDinamicallyAllocatedBuffer->getSize() << std::endl;

    //add numbers to buffers
    int number = 7;

    if( myStaticallyAllocatedBuffer.setData(0, number) )
        std::cout << "number " << number << " added to static buffer" << std::endl;
    else std::cout << "number not added to static buffer" << std::endl;

    if( myDinamicallyAllocatedBuffer->setData(9, number) )
        std::cout << "number " << number << " added to dynamic buffer" << std::endl;
    else std::cout << "number not added to dynamic buffer" << std::endl;

    // get numbers from buffers
    int num1, num2;
    myStaticallyAllocatedBuffer.getData(0, num1);
    myDinamicallyAllocatedBuffer->getData(10, num2);

    std::cout << "static buffer contain in position 0: " << num1 << std::endl;
    std::cout << "dynamic buffer contain in position 10: " << num2 << std::endl;

    // let's test copy constructor
    Buffer copyOfStaticBuf = myStaticallyAllocatedBuffer;

    //let's test plus "+" overridden operator
    Buffer staticPlusDynamic = myStaticallyAllocatedBuffer + *myDinamicallyAllocatedBuffer;
    std::cout << "size of staticPlusDynamic buffer: " << staticPlusDynamic.getSize() << std::endl;

    // let's test move constructor
    Buffer resultOfMoveBuffer = giveMeABuffer(1);

    return 0;
}


//used just to test the move constructor
Buffer giveMeABuffer(int size){
    Buffer theBuffer = Buffer(size);
    return theBuffer; //here the compiler **may*** call the move constructor
}