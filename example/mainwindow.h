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
    int counter;
    std::vector<QPlot*> plots;

    QPointF newSinPoint(int);
    QPlot* createPlot(void);
};

#endif // MAINWINDOW_H
