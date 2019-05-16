//
// Created by giovanni on 16/05/19.
//

#ifndef LAB2_FSEXCEPTION_H
#define LAB2_FSEXCEPTION_H

#include <iostream>
#include <exception>

struct FsException : public std::exception
{
    const char * what () const throw ()
    {
        return "Exception occured. exiting.";
    }
};


#endif //LAB2_FSEXCEPTION_H
