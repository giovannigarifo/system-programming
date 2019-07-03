//
// Created by giovanni on 03/07/19.
//

#include <fstream>
#include "AsyncStringSearcher.h"
#include "FileSearcherException.h"

AsyncStringSearcher::AsyncStringSearcher(string desiredString, vector<string> inputFiles) {

    futures = vector<future<tuple<thread::id, int>>>();

    for(string file : inputFiles){

        future<tuple<thread::id, int>> future = async(&AsyncStringSearcher::searchInFile, this, desiredString, file);
        futures.push_back(std::move(future));
    }
}


/**
 * Function executed by each worker
 * @param desiredString
 * @param fileName
 * @param searchResults : the list is passes as reference
 */
tuple<thread::id, int> AsyncStringSearcher::searchInFile(string desiredString, string fileName) {

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

    return make_tuple(this_thread::get_id(), num);
}

AsyncStringSearcher::~AsyncStringSearcher() {

    for(auto it = futures.begin(); it < futures.end(); it++){
        auto result = it->get();
        cout << "Async Thread TID=" << get<0>(result) << " found the string " << get<1>(result) << " times." << endl;
    }

}
