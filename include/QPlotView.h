#ifndef QPLOT_H
#define QPLOT_H

#include <QChartView>
#include <QDateTime>
#include <vector>
#include <tuple>

QT_CHARTS_USE_NAMESPACE

class QPlotView : public QChartView
{
    Q_OBJECT

public:
    typedef enum {
        BOOL,
        INTEGER,
        DOUBLE,
        CUSTOM_CATEGORY
    } Type;

    QPlotView(std::string title, Type type, QPlotView*& currentZoomPlot, bool updateOnAppend = true, std::vector<std::string>* custom = nullptr, QWidget *parent = 0);

    void append(bool);
    void append(int);
    void append(double);

    void setRecordOnAppend(bool);
    void setUpdateOnAppend(bool);
    QDateTime getMinTime(void);
    QDateTime getMaxTime(void);

signals:
    void signal_append(QDateTime dateTime, bool value);
    void signal_append(QDateTime dateTime, int value);
    void signal_append(QDateTime dateTime, double value);

public slots:
    void slot_append(QDateTime dateTime, bool value);
    void slot_append(QDateTime dateTime, int value);
    void slot_append(QDateTime dateTime, double value);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    std::string title;
    Type type;
    QPlotView*& currentZoomPlot;
    bool recordOnAppend;
    bool updateOnAppend;

    qreal lastMouseXPos;
    QDateTime minTime;
    QDateTime maxTime;
    std::vector<std::tuple<QDateTime, qreal>> buffer;

    void zoomW(qreal factor, qreal xPos);
};

#endif // QPLOT_H
