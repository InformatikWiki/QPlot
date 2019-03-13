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

    auto plot = createBoolPlot();
    plots.push_back(plot);
    layout->addWidget(plot);

    plot = createIntegerPlot();
    plots.push_back(plot);
    layout->addWidget(plot);

    plot = createDoublePlot();
    plots.push_back(plot);
    layout->addWidget(plot);

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

QPlot* MainWindow::createBoolPlot(void)
{
    QPlot *plot = new QPlot("BOOL Plot", QPlot::Type::BOOL);
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

QPlot* MainWindow::createIntegerPlot(void)
{
    QPlot *plot = new QPlot("INTEGER Plot", QPlot::Type::INTEGER);
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

QPlot* MainWindow::createDoublePlot(void)
{
    QPlot *plot = new QPlot("DOUBLE Plot", QPlot::Type::DOUBLE);
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
    if (counter < 600)
    {
        qDebug() << "QPlot Update...";

        plots.at(2)->append(qSin(M_PI / 50 * counter++) * 150);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{

}
