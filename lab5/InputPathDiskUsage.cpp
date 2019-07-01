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

    //connect to signals
    connect(Model::getModel(), SIGNAL(pathChanged(const QString&)), this, SLOT(updatePathDiskUsage(const QString&)));
}


/**
 * slot called when the inputPath in the modle has changed, it's subscribed to the
 * pathChanged signal of Model.
 */
void InputPathDiskUsage::updatePathDiskUsage(const QString& newPath) {

    //remove current series
    pieChart->removeAllSeries();
    series = new QPieSeries(this);

    //add new series
    vector<DirectoryEntry>* entries = Model::getModel()->getDirectoryInfo();
    for(const DirectoryEntry& entry : *entries){
        QPieSlice* slice = new QPieSlice(QString::fromStdString(entry.name),entry.size, series);
        slice->setLabelVisible(true);
        series->append(slice);
    }
    pieChart->addSeries(series);
}