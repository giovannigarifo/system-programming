#include <iostream>
#include <ctime>
#include "Buffer.h"

#define BUFSIZE 10
#define NUM_BENCH 10

//used just to test the move constructor
Buffer giveMeABuffer(int size);
void benchmark(Buffer& benchBuffer);

int main() {

    std::cout << "\nHello, let's create some buffers!\n" << std::endl;

    Buffer myStaticallyAllocatedBuffer(BUFSIZE);
    Buffer* myDinamicallyAllocatedBuffer = new Buffer(BUFSIZE);

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

    // let's test move constructor
    Buffer theMovedBuffer = giveMeABuffer(BUFSIZE);

    //let's test plus "+" overridden operator
    Buffer staticPlusDynamic = myStaticallyAllocatedBuffer + *myDinamicallyAllocatedBuffer;
    std::cout << "size of staticPlusDynamic buffer: " << staticPlusDynamic.getSize() << std::endl;

    //let's test the assignment operator that has been overridden
    theMovedBuffer = copyOfStaticBuf;

    /* Benchmarks */
    std::cout << "\nNow compare speed of move and copy:\n" << std::endl;

    Buffer** benchBuffers = new Buffer*[NUM_BENCH];
    int benchBuffSize = BUFSIZE;
    for(int i=0; i< NUM_BENCH; i++){
        benchBuffers[i] = new Buffer(benchBuffSize);
        benchBuffSize = benchBuffSize*10;
    }

    for(int i=0; i<NUM_BENCH; i++){
        std::cout << "\nRound "<<i+1<<" of benchmarks \n" << std::endl;
        benchmark(*benchBuffers[i]);
    }


    return 0;
}


//used just to test the move constructor
Buffer giveMeABuffer(int size){
    Buffer theBuffer = Buffer(size);
    return theBuffer; //here the compiler **may*** call the move constructor
}


#pragma optimize("", off ) // turn off all compiler optimizations
void benchmark(Buffer& benchBuffer){

    clock_t start; //used for benchmark

    //let's benchmark a copy
    start = std::clock();
    Buffer copytemp = benchBuffer; //copy
    std::cout<<"- Copy time: "<< ( std::clock() - start ) / (double) CLOCKS_PER_SEC <<'\n';

    //let's benchmark a move
    start = std::clock();
    Buffer movetemp = std::move(benchBuffer); // move
    std::cout<<"- Move time: "<< ( std::clock() - start ) / (double) CLOCKS_PER_SEC <<'\n';
}