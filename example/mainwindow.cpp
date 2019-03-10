#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>

#include "QPlot.h"

#include <QtMath>
#include <QLineSeries>
#include <QValueAxis>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      counter(0)
{
    auto layout = new QVBoxLayout();

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    auto plot = createPlot();
    plots.push_back(plot);
    layout->addWidget(plot);

    plot = createPlot();
    plots.push_back(plot);
    layout->addWidget(plot);

    timerId = startTimer(1000);

    resize(600, 500);
    show();
}

MainWindow::~MainWindow()
{

}

QPointF MainWindow::newSinPoint(int c)
{
    return QPointF((qreal)c, qSin(M_PI / 50 * c) * 100);
}

QPlot* MainWindow::createPlot(void)
{
    QLineSeries *series = new QLineSeries();
    for (counter = 0; counter < 500; counter++)
    {
        series->append(newSinPoint(counter));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Plot mouse Zoom In/Out");
    //chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->createDefaultAxes();

    QPlot *plot = new QPlot(chart);
    plot->setRenderHint(QPainter::Antialiasing);
    return plot;
}

void MainWindow::timerEvent(QTimerEvent*)
{
    qDebug() << "QPlot Update...";

    QChart* chart = plots.at(0)->chart();
    QLineSeries *series = dynamic_cast<QLineSeries*>(chart->series().at(0));
    series->append(newSinPoint(counter++));
}
