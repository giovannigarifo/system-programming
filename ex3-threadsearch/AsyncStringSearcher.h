//
// Created by giovanni on 03/07/19.
//

#ifndef EX3_THREADSEARCH_ASYNCSTRINGSEARCHER_H
#define EX3_THREADSEARCH_ASYNCSTRINGSEARCHER_H

#include <future>
#include <vector>

using namespace std;

class AsyncStringSearcher {

private:
    vector<future<tuple<thread::id, int>>> futures;
    tuple<thread::id, int> searchInFile(string desiredString, string fileName); // function executed by each thread, it searches in a single file

public:
    virtual ~AsyncStringSearcher();

    AsyncStringSearcher(string desiredString, vector<string> inputFiles);
};


#endif //EX3_THREADSEARCH_ASYNCSTRINGSEARCHER_H
