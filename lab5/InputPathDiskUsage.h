//
// Created by giovanni on 30/06/19.
//

#ifndef LAB5_INPUTPATHDISKUSAGE_H
#define LAB5_INPUTPATHDISKUSAGE_H

#include <QChartView>
#include <QBarSet>
#include <QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QPieSeries>

using namespace QtCharts;

class InputPathDiskUsage : public QWidget{
    Q_OBJECT

private:
    QString partialTitle;
    QPieSeries* series;
    QChart* pieChart;
    QChartView* pieChartView;
    QHBoxLayout* layout;

public:
    explicit InputPathDiskUsage(QWidget *parent);
};


#endif //LAB5_INPUTPATHDISKUSAGE_H
