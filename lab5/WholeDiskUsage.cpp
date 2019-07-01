//
// Created by giovanni on 30/06/19.
//

#include <QtCore/QStorageInfo>
#include <cmath>
#include "WholeDiskUsage.h"

using namespace std;

WholeDiskUsage::WholeDiskUsage(QWidget *parent) : QWidget(parent) {

    //load disk usage info
    qint64 freeGB = 0;
    qint64 totalGB = 0;
    QStorageInfo* storageInfo = new QStorageInfo("/");

    //create series
    series = new QHorizontalStackedBarSeries(this);

    //create barSets

    // root info
    freeGB = storageInfo->bytesFree() / pow(2,30);
    totalGB = storageInfo->bytesTotal() / pow(2,30);

    gbTotal = new QBarSet("", this);
    *gbTotal << totalGB;
    QString totalLabel = QString::fromStdString("Total: ").append(QString::fromStdString(to_string(totalGB)));
    gbTotal->setLabel(totalLabel);

    gbUsed = new QBarSet("", this);
    *gbUsed << totalGB-freeGB;
    QString usedLabel = QString::fromStdString("Used: ").append(QString::fromStdString(to_string(totalGB-freeGB)));
    gbUsed->setLabel(usedLabel);

    series->append(gbTotal);
    series->append(gbUsed);

    //create chart
    barChart = new QChart(); //TODO: take a parent different from QWidget
    barChart->addSeries(series);
    barChart->setTitle("Root partition usage in GB");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

    //set rendered for the chart view
    barChartView = new QChartView(barChart, this);
    barChartView->setRenderHint(QPainter::Antialiasing);

    layout = new QHBoxLayout(this);
    layout->addWidget(barChartView);
    layout->setMargin(0);
    this->setLayout(layout);
    this->setFixedHeight(170);
}
