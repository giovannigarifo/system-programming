#include <iostream>

#include "StringSearcher.h"


int main() {

    auto files = vector<string>();
    files.emplace_back("test1.txt");
    files.emplace_back("test2.txt");
    files.emplace_back("test3.txt");

    StringSearcher ss = StringSearcher("test1", files);

    return 0;
}