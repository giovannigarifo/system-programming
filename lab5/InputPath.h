//
// Created by giovanni on 29/06/19.
//

#ifndef LAB5_INPUTPATH_H
#define LAB5_INPUTPATH_H

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QHBoxLayout>
#include <QCompleter>
#include <QtWidgets/QDirModel>
#include "PathValidator.h"


using namespace std;

class InputPath  : public QWidget {
    Q_OBJECT

private:
    QString inputPath;
    QLineEdit* lineEdit;
    QPushButton* pushButton;
    QHBoxLayout* layout;
    QCompleter* completer;
    PathValidator* validator;

public slots:
    void enterPressed();
    void completionSelected(const QString& input);
    void wrongInput();

public:
    explicit InputPath(QWidget *parent);
};


#endif //LAB5_INPUTPATH_H
