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
    void wheelEvent(QWheelEvent *event);

private:
    int timerId;
    int counter;
    QPlot<bool>* boolPlot;
    QPlot<int>* intPlot;
    QPlot<double>* doublePlot;

    QPointF newSinPoint(int);
    QPlot<bool>* createBoolPlot(void);
    QPlot<int>* createIntegerPlot(void);
    QPlot<double>* createDoublePlot(void);
};

#endif // MAINWINDOW_H
