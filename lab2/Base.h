//
// Created by giovanni.garifo@outlook.com on 13/05/19.
//

#ifndef LAB2_BASE_H
#define LAB2_BASE_H
#include <string>


/*
 * Abstract class Base
 */
class Base {

private:
    std::string name;

public:
    std::string getName() const;
    void setName(std::string name);
    virtual ~Base() = default;
    virtual int mType() const = 0; //pure virtual function
    virtual void ls(int indent) const = 0;
};


#endif //LAB2_BASE_H
