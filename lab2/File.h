//
// Created by giovanni on 15/05/19.
//

#ifndef LAB2_FILE_H
#define LAB2_FILE_H

#include "Base.h"
#include "FsException.h"

using namespace std;

class File: public Base {

private:
    uintmax_t size;

public:
    File(string name, uintmax_t size);

    uintmax_t getSize() const;
    void ls(int indent) const override;

    int mType() const override;
};


#endif //LAB2_FILE_H
