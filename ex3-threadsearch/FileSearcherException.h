//
// Created by giovanni on 26/05/19.
//

#ifndef EX3_THREADSEARCH_FILESEARCHEREXCEPTION_H
#define EX3_THREADSEARCH_FILESEARCHEREXCEPTION_H

#include <iostream>
#include <exception>

class FileSearcherException : public std::exception {

public:
    const char * what () const throw ()
    {
        return "FileSearcherException exception occured.";
    }
};


#endif //EX3_THREADSEARCH_FILESEARCHEREXCEPTION_H
