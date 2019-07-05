#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

class Stack {

private:
    vector<int>* stack;
    int max_dim;
    mutex m;
    condition_variable stackFull;

public:
    Stack(int max_dim):max_dim(max_dim){stack = new vector<int>();};
    ~Stack(){ delete[] stack;}

    //pushes an element into the stack, if not full
    void push(int elem){

        unique_lock<mutex> ul(m);
        stackFull.wait(ul, [this]() -> bool {return (stack->size() != max_dim);});

        stack->push_back(elem);
    }

    //pop an element from the stack, if there is one
    int pop(){

        lock_guard<mutex> lg(m);

        if(stack->empty())
            return -1;

        int back = stack->back();
        stack->pop_back();
        stackFull.notify_all();
        return back;
    }
};


int main(int argc, char** argv){

    int num_threads = 5;
    int max_dim = 10;

    Stack stack(max_dim);
    vector<thread> threads = vector<thread>();

    for(int i=0; i<num_threads; i++){
        threads.push_back(thread([&stack,i](){
            stack.push(i);
            cout << "popped: " << stack.pop() <<endl;
        }));
    }

    for(auto iter = threads.begin(); iter < threads.end(); iter++)
        (*iter).join();

    return 0;
}
