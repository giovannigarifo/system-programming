//
// Created by giovanni on 26/05/19.
//

#include "ThreadStringSearcher.h"
#include "FileSearcherException.h"
#include <sstream>

mutex tuple_vector_mutex;


// Constructor
ThreadStringSearcher::ThreadStringSearcher(string desiredString, vector<string> inputFiles){

    this->workers = list<thread>();
    this->searchResults = tuple_vector();

    for (string inputFile : inputFiles)
        this->workers.push_back(std::move(thread(&ThreadStringSearcher::searchInFile,
                                                 this, desiredString,
                                                 inputFile,
                                                 std::ref((this->searchResults))))); //passed as reference
}

// Destructor implements RAII
ThreadStringSearcher::~ThreadStringSearcher() {

    for (auto &worker : workers)
        if(worker.joinable())
            worker.join();

    tuple_vector_mutex.lock();

    for(auto result : searchResults)
        cout << "Thread TID=" << get<0>(result) << " found the string " << get<1>(result) << " times." << endl;

    tuple_vector_mutex.unlock();
}


/**
 * Function executed by each worker
 * @param desiredString
 * @param fileName
 * @param searchResults : the list is passes as reference
 */
void ThreadStringSearcher::searchInFile(string desiredString, string fileName, tuple_vector& searchResults) {

    ifstream ifs;
    int num = 0;

    try {

        ifs.open(fileName);

        if (ifs.is_open()){

            string line;

            while(getline(ifs, line) && ifs.good()){

                unsigned long pos = 0;

                while((pos = line.find(desiredString, pos)) != string::npos){
                    num += 1;
                    pos += desiredString.size();
                }

            }

        } else {
            throw FileSearcherException(); //not the best thing to throw directly inside the try, it's just for fun
        }

    } catch(FileSearcherException& e1){
        cout << "FileSearcherException: " << e1.what() << endl;
    } catch(exception& e2) {
        cout << "exception:" << e2.what() << endl;
    }

    // update global result
    tuple_vector_mutex.lock();

    searchResults.push_back(make_tuple(this_thread::get_id(), num));

    tuple_vector_mutex.unlock();
}
