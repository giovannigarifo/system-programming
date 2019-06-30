//
// Created by giovanni on 29/06/19.
//

#ifndef LAB5_INPUTPATH_H
#define LAB5_INPUTPATH_H

#include <QtWidgets/QLineEdit>

using namespace std;

class InputPath  : public QLineEdit {
    Q_OBJECT

private:
    QString inputPath;

public:
    InputPath(const QString &string, QWidget *parent);
};


#endif //LAB5_INPUTPATH_H
