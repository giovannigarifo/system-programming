//
// Created by giovanni on 30/06/19.
//

#ifndef LAB5_PATHVALIDATOR_H
#define LAB5_PATHVALIDATOR_H


#include <QtGui/QValidator>

class PathValidator : public QValidator{

public:
    PathValidator(QObject *parent);

    State validate(QString &string, int &i) const override;
};


#endif //LAB5_PATHVALIDATOR_H
