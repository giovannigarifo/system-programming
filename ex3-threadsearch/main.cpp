#include <iostream>

#include "ThreadStringSearcher.h"
#include "AsyncStringSearcher.h"


int main() {

    auto files = vector<string>();
    files.emplace_back("test1.txt");
    files.emplace_back("test2.txt");
    files.emplace_back("test3.txt");

    bool asyncMode = true; //for poor man switch

    if(asyncMode)
        AsyncStringSearcher ass = AsyncStringSearcher("test1", files); //TODO: change object name :)
    else
        ThreadStringSearcher tss = ThreadStringSearcher("test1", files);

    return 0;
}