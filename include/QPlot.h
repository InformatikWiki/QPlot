#ifndef QPLOT_H
#define QPLOT_H

#include <QChartView>
#include <QTimer>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE

class QPlot : public QChartView
{
    Q_OBJECT

public:
    typedef enum {
        BOOL,
        INTEGER,
        DOUBLE
    } Type;

    QPlot(std::string title, Type type, QWidget *parent = 0);

    void append(bool);
    void append(int);
    void append(double);

protected:
    void updateAllMinMaxTime(void);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    std::string title;
    Type type;
    qreal lastMouseXPos;
    int counter;
    int timerId;
    QDateTime minTime;
    QDateTime maxTime;

    static QDateTime allMinTime;
    static QDateTime allMaxTime;


    static QPlot*    currentZoomPlot;
    static QDateTime currentZoomMinTime;
    static QDateTime currentZoomMaxTime;

    void zoomW(qreal factor, qreal xPos);
};

#endif // QPLOT_H
