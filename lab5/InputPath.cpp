//
// Created by giovanni on 29/06/19.
//

#include "InputPath.h"


InputPath::InputPath(QWidget *parent) : QWidget(parent) {
    QStringList wordList;

    inputPath = Model::getModel()->getInputPath();

    //setup line edit with autocompletion
    lineEdit = new QLineEdit(inputPath, this);

    completer = new QCompleter(this);
    validator = new PathValidator(lineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(new QDirModel(completer));

    lineEdit->setCompleter(completer);
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setValidator(validator);

    connect(lineEdit,SIGNAL(editingFinished()), this, SLOT(enterPressed()));
    connect(completer,SIGNAL(activated(const QString&)), this, SLOT(completionSelected(const QString&)));
    connect(lineEdit, SIGNAL(inputRejected()), this, SLOT(wrongInput()));

    //setup pushbutton
    pushButton = new QPushButton("Up",this);

    //setup layout
    layout = new QHBoxLayout(this);
    layout->addWidget(lineEdit);
    layout->addWidget(pushButton);

    this->setLayout(layout);
}

/**
 * slot called when the editingFinished signal is emitted by the QLineERdit
 */
void InputPath::enterPressed() {

    Model::getModel()->setInputPath(lineEdit->text()); //the input path selected by pressing enter
}

/**
 * slot called when the activated signal is emitted by the QCompleter
 *
 * @param input the input path selected by the dropdown menu
 */
void InputPath::completionSelected(const QString& input) {

    Model::getModel()->setInputPath(input);
}

/**
 * slot called when the inputRejected signal has been emitted by the QlineEdit,
 * this signal is emitted when the QValidator associated return "Invalid"
 */
void InputPath::wrongInput() {
    qDebug() << "wrong input path inserted by the user.";
}