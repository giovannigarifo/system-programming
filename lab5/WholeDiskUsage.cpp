//
// Created by giovanni on 30/06/19.
//

#include "WholeDiskUsage.h"

WholeDiskUsage::WholeDiskUsage(QWidget *parent) : QWidget(parent) {

    //create series
    series = new QHorizontalStackedBarSeries(this);

    //create barSet
    gbUsed = new QBarSet("GB", this);
    series->append(gbUsed);

    //create chart
    barChart = new QChart(); //TODO: take a parent different from QWidget
    barChart->addSeries(series);
    barChart->setTitle("Disk Usage");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

    //set rendered for the chart view
    barChartView = new QChartView(barChart, this);
    barChartView->setRenderHint(QPainter::Antialiasing);

    layout = new QHBoxLayout(this);
    layout->addWidget(barChartView);
    this->setLayout(layout);

}
