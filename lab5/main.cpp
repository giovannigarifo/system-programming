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
#include <QtCharts/QPieSeries>
#include "InputPath.h"
#include "WholeDiskUsage.h"
#include "InputPathDiskUsage.h"

//TODO: aggiungere i `.h` in add_executable in cmakelists

using namespace std;
using namespace QtCharts;

int main(int argc, char** argv) {

    // initialize Qt
    QApplication app(argc, argv);

    // root of view tree
    QWidget *window = new QWidget();

    // bar chart for disk usage
    WholeDiskUsage* wdu = new WholeDiskUsage(window);

    // user input
    InputPath* ip = new InputPath(window);

    // Pie Chart that display disk usage of input path
    InputPathDiskUsage* ipdu = new InputPathDiskUsage(window);

    // assemble view tree
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(wdu);
    mainLayout->addWidget(ip);
    mainLayout->addWidget(ipdu);
    window->setLayout(mainLayout);


    // show view tree and start the message queue loop
    window->resize(600, window->height());
    window->show();
    return QApplication::exec();
}