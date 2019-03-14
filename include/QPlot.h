#ifndef QPLOT_H
#define QPLOT_H

#include <QChartView>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE

template<class T>
class QPlot : public QChartView
{
public:
    QPlot(std::string title, QWidget *parent = 0);

    void append(T);

protected:
    void initAxis(void);
    qreal updateTime(void);
    void updateAllMinMaxTime(void);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    std::string title;
    qreal lastMouseXPos;
    int counter;
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
