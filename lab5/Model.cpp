//
// Created by giovanni on 30/06/19.
//

#include "Model.h"

//required initialization for singleton
Model* Model::model = NULL;


Model::Model() : QFileSystemModel() {

    //initially use current path
    inputPath = QDir::currentPath();
}

Model* Model::getModel() {

    if(!model)
       model = new Model();

    return model;
}

QString Model::getInputPath() {
    return inputPath;
}

void Model::setInputPath(QString input) {
    inputPath = input;
}