#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHorizontalStackedBarSeries>
#include <QBarSet>
#include <QChart>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include "InputPath.h"


//TODO: aggiungere i `.h` in add_executable in cmakelists

using namespace std;
using namespace QtCharts;

int main(int argc, char** argv) {

    // initialize Qt
    QApplication app(argc, argv);

    //root of view tree
    QWidget *window = new QWidget();

    //layout
    QVBoxLayout *layout = new QVBoxLayout();

    //line edit component
    InputPath* inputPath = new InputPath("Insert path here", window);

    //horizontal bar (disk space usage)
    QHorizontalStackedBarSeries* series = new QHorizontalStackedBarSeries();
    QBarSet *set0 = new QBarSet("Jane");
    series->append(set0);

    QChart *barChart = new QChart();
    barChart->addSeries(series);
    barChart->setTitle("Disk Usage");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(barChart);
    chartView->setRenderHint(QPainter::Antialiasing);



    //assemble view tree
    window->setLayout(layout);
    layout->addWidget(inputPath);
    layout->addWidget(chartView);


    //show view tree and start the message queue loop
    window->resize(500, window->height());
    window->show();
    return QApplication::exec();
}