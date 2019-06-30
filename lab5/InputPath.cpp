//
// Created by giovanni on 29/06/19.
//

#include "InputPath.h"

InputPath::InputPath(const QString &string, QWidget *parent) : QLineEdit(string, parent) {
    this->inputPath = string;
}
