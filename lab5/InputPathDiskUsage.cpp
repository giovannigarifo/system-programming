//
// Created by giovanni on 30/06/19.
//

#include "InputPathDiskUsage.h"
#include "Model.h"

InputPathDiskUsage::InputPathDiskUsage(QWidget *parent) : QWidget(parent) {

    series = new QPieSeries(this);

    pieChart = new QChart(); //TODO: take a parent different from QWidget
    pieChart->addSeries(series);

    partialTitle = "Disk Usage for: ";
    partialTitle.append(Model::getModel()->getInputPath());

    pieChart->setTitle(partialTitle);
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    pieChartView = new QChartView(pieChart, this);
    pieChartView->setRenderHint(QPainter::Antialiasing);

    layout = new QHBoxLayout(this);
    layout->addWidget(pieChartView);
    this->setLayout(layout);
}