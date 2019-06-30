//
// Created by giovanni on 29/06/19.
//

#ifndef LAB5_INPUTPATH_H
#define LAB5_INPUTPATH_H

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QHBoxLayout>

using namespace std;

class InputPath  : public QWidget {
    Q_OBJECT

private:
    QString inputPath;
    QLineEdit* lineEdit;
    QPushButton* pushButton;
    QHBoxLayout* layout;

public:
    explicit InputPath(QWidget *parent);
};


#endif //LAB5_INPUTPATH_H
