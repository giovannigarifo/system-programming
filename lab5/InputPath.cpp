//
// Created by giovanni on 29/06/19.
//

#include "InputPath.h"

InputPath::InputPath(QWidget *parent) : QWidget(parent) {

    inputPath = "path here";
    lineEdit = new QLineEdit(inputPath, this);
    pushButton = new QPushButton("Up",this);
    layout = new QHBoxLayout(this);

    layout->addWidget(lineEdit);
    layout->addWidget(pushButton);

    this->setLayout(layout);
}
