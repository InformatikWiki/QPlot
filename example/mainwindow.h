#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "QPlot.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event);

private:
    int timerId;
    QPlot* plot;
    QPlotView* boolView;
    QPlotView* intView;
    QPlotView* doubleView1;
    QPlotView* doubleView2;
    QPlotView* customView;
    int counter;
};

#endif // MAINWINDOW_H
