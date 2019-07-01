//
// Created by giovanni on 30/06/19.
//

#include <QtCore/QDir>
#include "PathValidator.h"

PathValidator::PathValidator(QObject *parent) : QValidator(parent) {}

/**
 * validate the inserted path
 * @param path
 * @param i
 * @return the result of validation
 */
QValidator::State PathValidator::validate(QString &path, int &i) const {

    QDir pathDir(path);

    if(path.endsWith("/")){
        if(pathDir.exists())
            return Acceptable;
        else return Invalid;
    }

    if(pathDir.exists())
        return Acceptable;
    else return Intermediate;
}
