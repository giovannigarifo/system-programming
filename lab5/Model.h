//
// Created by giovanni on 30/06/19.
//

#ifndef LAB5_MODEL_H
#define LAB5_MODEL_H

#include <QtCore/QString>
#include <QtWidgets/QFileSystemModel>


// singleton
class Model : public QFileSystemModel {
    Q_OBJECT

private:
    QString inputPath;
    static Model* model;

    explicit Model(); //cosntructor

public:
    Model(const Model& source) = delete;
    Model(Model&& source) = delete;
    static Model* getModel();

    QString getInputPath();
    void setInputPath(QString input);
};


#endif //LAB5_MODEL_H
