#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>

#include "QPlot.h"
#include <QtMath>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    plot = new QPlot();
    setCentralWidget(plot);

    int sampleDelayMs = 1;

    // Create and init bool view
    boolView = plot->addView("bool plot", QPlotView::Type::BOOL);
    boolView->append(false);

    // Create and init int view
    intView = plot->addView("int plot", QPlotView::Type::INTEGER);
    intView->append(0);

    // Create and init double view 1
    doubleView1 = plot->addView("double plot", QPlotView::Type::DOUBLE);
    doubleView1->append(0.0);

    // Create and init double view 2
    doubleView2 = plot->addView("double plot", QPlotView::Type::DOUBLE);
    doubleView2->append(0.0);

    // Create and init custom view
    std::vector<std::string> custom;
    custom.push_back("Value 1"); // 0 -> "Value 1"
    custom.push_back("Value 2"); // 1 -> "Value 2"
    custom.push_back("Value 3"); // ...
    custom.push_back("Value 4");
    customView = plot->addView("custom plot", QPlotView::Type::CUSTOM_CATEGORY, &custom);
    customView->append(0);

    // Fill bool view
    bool val = false;
    for (counter = 0; counter < 500; counter++)
    {
        boolView->append(val);
        if (counter % 4 == 0)
            val = !val;
        QThread::msleep(sampleDelayMs);
    }

    // Fill int view
    for (counter = 0; counter < 200; counter++)
    {
        intView->append(counter);
        QThread::msleep(sampleDelayMs);
    }

    // Fill double view 1
    for (counter = 0; counter < 500; counter++)
    {
        doubleView1->append(qSin(M_PI / 50 * counter) * 100);
        QThread::msleep(sampleDelayMs);
    }

    // Fill double view 2
    for (counter = 0; counter < 300; counter++)
    {
        doubleView2->append(qSin(M_PI / 50 * counter) * 100);
        QThread::msleep(sampleDelayMs);
    }

    // Fill custom view
    for (counter = 0; counter < 200; counter++)
    {
        customView->append(counter%4);
        QThread::msleep(sampleDelayMs);
    }

    counter = 0;
    timerId = startTimer(100);

    resize(600, 500);
    show();
}

MainWindow::~MainWindow()
{

}

void MainWindow::timerEvent(QTimerEvent*)
{
    if (counter < 1000)
    {
        boolView->append(counter % 4 == 0 ? true : false);
        intView->append(counter);
        doubleView1->append(qSin(M_PI / 50 * counter) * 150);
        doubleView2->append(qSin(M_PI / 50 * counter) * 150);
        customView->append(counter%4);
        counter++;
    }
}
