//
// Created by giovanni on 30/06/19.
//

#include <cmath>
#include "Model.h"

//required initialization for singleton
Model* Model::model = NULL;


Model::Model() : QObject() {

    //initially use current path
    inputPath = QDir::currentPath();
    totPathSize = 0;
    directoryInfo = new vector<DirectoryEntry>();
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

    if(input == inputPath)
        return;

    inputPath = std::move(input);
    emit pathChanged(inputPath);
}

vector<DirectoryEntry>* Model::getDirectoryInfo(){

    QDirIterator* dirIter = new QDirIterator(inputPath);

    while(dirIter->hasNext()){

        dirIter->next();
        QFileInfo fileInfo = dirIter->fileInfo();
        DirectoryEntry entry;

        entry.name = fileInfo.fileName().toStdString();

        //skip current and parent directories
        if(entry.name == "." || entry.name == "..")
            continue;

        if(fileInfo.isDir())
            entry.isDir = true;

        entry.size = (float)calcFileOrDirSize(fileInfo)/(pow(2, 20)); //convert to MB
        directoryInfo->push_back(entry);

        totPathSize += entry.size;
    }

    delete dirIter;
    return directoryInfo;
}

qint64 Model::calcFileOrDirSize(QFileInfo dir) {

    if(!dir.isDir())
        return dir.size();
    else {

        QDirIterator* dirIter = new QDirIterator(dir.absoluteFilePath());
        qint64 size = 0;

        while(dirIter->hasNext()){
            dirIter->next();

            //skip current and parent directories
            if(dirIter->fileName() == "." || dirIter->fileName() == "..")
                continue;

            size += calcFileOrDirSize(dirIter->fileInfo());
        }

        delete dirIter;
        return size;
    }
}

Model::~Model() {
    delete[] directoryInfo;
}

qint64 Model::getTotPathSize() const {
    return totPathSize;
}

void Model::setTotPathSize(qint64 totPathSize) {
    Model::totPathSize = totPathSize;
}
