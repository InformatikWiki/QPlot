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
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      counter(0)
{
    auto layout = new QVBoxLayout();

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    boolPlot = createBoolPlot();
    layout->addWidget(boolPlot);

    intPlot = createIntegerPlot();
    layout->addWidget(intPlot);

    doublePlot = createDoublePlot();
    layout->addWidget(doublePlot);

    counter = 100;
    timerId = startTimer(100);

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

QPlot<bool>* MainWindow::createBoolPlot(void)
{
    QPlot<bool>* plot = new QPlot<bool>("BOOL Plot");
    plot->setRenderHint(QPainter::Antialiasing);
    bool val = false;
    for (counter = 0; counter < 500; counter++)
    {
        plot->append(val);
        if (counter % 4 == 0)
            val = !val;
        QThread::msleep(10);
    }
    return plot;
}

QPlot<int>* MainWindow::createIntegerPlot(void)
{
    QPlot<int>* plot = new QPlot<int>("INTEGER Plot");
    plot->setRenderHint(QPainter::Antialiasing);
    counter = 0;
    while (counter < 200)
    {
        plot->append(counter);
        counter += 1;
        QThread::msleep(10);
    }
    return plot;
}

QPlot<double>* MainWindow::createDoublePlot(void)
{
    QPlot<double>* plot = new QPlot<double>("DOUBLE Plot");
    plot->setRenderHint(QPainter::Antialiasing);

    for (counter = 0; counter < 500; counter++)
    {
        plot->append(qSin(M_PI / 50 * counter) * 100);
        QThread::msleep(10);
    }
    return plot;
}

void MainWindow::timerEvent(QTimerEvent*)
{
    if (counter < 100)
    {
        qDebug() << "QPlot Update...";

        doublePlot->append(qSin(M_PI / 50 * counter++) * 150);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{

}
