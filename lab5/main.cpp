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
#include "Model.h"

//TODO: aggiungere i `.h` in add_executable in cmakelists

using namespace std;
using namespace QtCharts;

int main(int argc, char** argv) {

    // initialize Qt
    QApplication app(argc, argv);

    // creates root component
    QWidget *window = new QWidget();

    // get model
    Model* model = Model::getModel();
    model->setParent(window);

    /**
     * Creates view components
     */
    // bar chart for disk usage
    WholeDiskUsage* wdu = new WholeDiskUsage(window);

    // user input
    InputPath* ip = new InputPath(window);

    // Pie Chart that display disk usage of input path
    InputPathDiskUsage* ipdu = new InputPathDiskUsage(window);

    // assemble view tree
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QVBoxLayout *subLayout1 = new QVBoxLayout();
    subLayout1->addWidget(ipdu);

    QHBoxLayout *subLayout2 = new QHBoxLayout();
    subLayout2->addWidget(wdu);
    subLayout2->addWidget(ip);

    mainLayout->addLayout(subLayout2);
    mainLayout->addLayout(subLayout1);

    window->setLayout(mainLayout);


    // show view tree and start the message queue loop
    //window->resize(600, 900);
    window->showMaximized();
    window->show();
    return QApplication::exec();
}