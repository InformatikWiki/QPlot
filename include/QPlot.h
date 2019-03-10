#ifndef QPLOT_H
#define QPLOT_H

#include <QChartView>
#include <QTimer>

QT_CHARTS_USE_NAMESPACE

class QPlot : public QChartView
{
    Q_OBJECT

public:
    QPlot(QChart *chart, QWidget *parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    qreal lastMouseXPos;
    int timerId;

    void zoomW(qreal factor, qreal xPos);
};

#endif // QPLOT_H
