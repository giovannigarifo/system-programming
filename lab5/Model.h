//
// Created by giovanni on 30/06/19.
//

#ifndef LAB5_MODEL_H
#define LAB5_MODEL_H

#include <QtCore/QString>
#include <QtWidgets/QFileSystemModel>
#include "DirectoryEntry.h"
#include <QStorageInfo>
#include <QDebug>

// singleton
class Model : public QObject {
    Q_OBJECT

private:
    QString inputPath;
    vector<DirectoryEntry>* directoryInfo;
    static Model* model;
    qint64 totPathSize;
public:
    qint64 getTotPathSize() const;

    void setTotPathSize(qint64 totPathSize);

private:
    explicit Model(); //cosntructor

    qint64 calcFileOrDirSize(QFileInfo dir);

public:
    Model(const Model& source) = delete;
    Model(Model&& source) = delete;
    static Model* getModel();

    QString getInputPath();
    void setInputPath(QString input);

    vector<DirectoryEntry>* getDirectoryInfo();

    ~Model() override;

signals:
    void pathChanged(const QString& newValue); //notifies that the path has changed
};


#endif //LAB5_MODEL_H
