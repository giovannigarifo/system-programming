//
// Created by giovanni on 26/05/19.
//

#ifndef EX3_THREADSEARCH_FILESEARCHER_H
#define EX3_THREADSEARCH_FILESEARCHER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <list>
#include <mutex>

using namespace std;

typedef list<tuple<thread::id, int >> tuple_vector;


class StringSearcher {

private:
    list<thread> workers; // list of threads that perform the search
    tuple_vector searchResults;
    void searchInFile(string desiredString, string fileName, tuple_vector& searcResults); // function executed by each thread, it searches in a single file

public:
    ~StringSearcher();
    StringSearcher(string desiredString, vector<string> inputFiles);

};


#endif //EX3_THREADSEARCH_FILESEARCHER_H
