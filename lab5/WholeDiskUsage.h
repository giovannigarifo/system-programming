//
// Created by giovanni on 30/06/19.
//

#ifndef LAB5_WHOLEDISKUSAGE_H
#define LAB5_WHOLEDISKUSAGE_H

#include <QChartView>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QBarSet>
#include <QChart>
#include <QtWidgets/QVBoxLayout>


using namespace QtCharts;

class WholeDiskUsage : public QWidget {
    Q_OBJECT

private:
    QHorizontalStackedBarSeries* series;
    QBarSet *gbUsed;
    QChart *barChart;
    QChartView* barChartView;
    QHBoxLayout* layout;

public:

    explicit WholeDiskUsage(QWidget *parent);

};


#endif //LAB5_WHOLEDISKUSAGE_H
