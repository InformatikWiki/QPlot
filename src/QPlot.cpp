#include "QPlot.h"
#include <QMouseEvent>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QLineSeries>
#include <QDebug>

template<class T>
QDateTime QPlot<T>::allMinTime = QDateTime::currentDateTime();
template<class T>
QDateTime QPlot<T>::allMaxTime = QDateTime::currentDateTime();
template<class T>
QPlot<T>* QPlot<T>::currentZoomPlot = nullptr;
template<class T>
QDateTime QPlot<T>::currentZoomMinTime = QDateTime::currentDateTime();
template<class T>
QDateTime QPlot<T>::currentZoomMaxTime = QDateTime::currentDateTime();

template<>
QPlot<bool>::QPlot(std::string title, QWidget *parent) :
    QChartView(new QChart(), parent),
    title(title),
    lastMouseXPos(0.0),
    counter(0)
{
    initAxis();
}

template<>
QPlot<int>::QPlot(std::string title, QWidget *parent) :
    QChartView(new QChart(), parent),
    title(title),
    lastMouseXPos(0.0),
    counter(0)
{
    initAxis();
}

template<>
QPlot<double>::QPlot(std::string title, QWidget *parent) :
    QChartView(new QChart(), parent),
    title(title),
    lastMouseXPos(0.0),
    counter(0)
{
    initAxis();
}

template<class T>
QPlot<T>::QPlot(std::string title, QWidget *parent) :
    QChartView(new QChart(), parent),
    title(title),
    lastMouseXPos(0.0),
    counter(0)
{
    initAxis();
}

template<class T>
void QPlot<T>::initAxis(void)
{
    QLineSeries *series = new QLineSeries();
    chart()->addSeries(series);
    chart()->setTitle(title.c_str());
    //chart()->setAnimationOptions(QChart::SeriesAnimations);
    chart()->legend()->hide();
    //chart()->createDefaultAxes();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm:ss.zzz");
    QDateTime now = QDateTime::currentDateTime();
    minTime = now;
    //axisX->setTitleText("Date");
    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->axisX()->setMin(minTime);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%.0f");
    axisY->applyNiceNumbers();

    //if (type == Type::BOOL)
    //{
    //    axisY->setTickCount(2);
    //}
    //axisY->setTitleText("Sunspots count");
    chart()->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    connect(series, &QLineSeries::pointAdded, [=](int index)
    {
        if (QValueAxis* axis = dynamic_cast<QValueAxis*>(chart()->axisY()))
        {
            qreal y = series->at(index).y();
            qreal yMax = axis->max();
            qreal yMin = axis->min();
            if ( (y < yMin) || (y > yMax) )
            {
                if (y < yMin)
                    yMin = y;
                if (y > yMax)
                    yMax = y;
                axis->setRange(yMin, yMax);
            }
        }
    });
}

template<>
void QPlot<bool>::append(bool value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        qreal xValue = updateTime();
        if (series->points().count() > 0)
        {
            QPointF last = series->points().last();
            bool lastBool = false;
            if (last.y() > 0.5)
            {
                lastBool = true;
            }
            if (value != lastBool)
            {
                series->append(QPointF(xValue, (lastBool ? 1.0 : 0.0)));
            }
        }
        series->append(QPointF(xValue, (value ? 1.0 : 0.0)));
    }
}

template<>
void QPlot<int>::append(int value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        qreal xValue = updateTime();
        if (series->points().count() > 0)
        {
            QPointF last = series->points().last();
            series->append(QPointF(xValue, last.y()));
        }
        series->append(QPointF(xValue, (qreal)value));
    }
}

template<>
void QPlot<double>::append(double value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        series->append(updateTime(), (qreal)value);
    }
}

template<class T>
qreal QPlot<T>::updateTime(void)
{
    maxTime = QDateTime::currentDateTime();
    updateAllMinMaxTime();

    chart()->axisX()->setMax(maxTime);
    return maxTime.toMSecsSinceEpoch();
}

template<class T>
void QPlot<T>::updateAllMinMaxTime(void)
{
    if (minTime < allMinTime)
        allMinTime = minTime;

    if (maxTime > allMaxTime)
        allMaxTime = maxTime;
}

template<class T>
void QPlot<T>::mouseMoveEvent(QMouseEvent *event)
{
    lastMouseXPos = event->localPos().x();
    QChartView::mouseMoveEvent(event);
}

template<class T>
void QPlot<T>::wheelEvent(QWheelEvent *event)
{
    currentZoomPlot = this;
    qreal factor=1.0;
    factor *= event->angleDelta().y() > 0 ? 0.5 : 2;
    zoomW(factor, lastMouseXPos);
}

template<class T>
void QPlot<T>::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Plus:
            zoomW(0.5, chart()->plotArea().center().x());
            break;
        case Qt::Key_Minus:
            zoomW(2.0, chart()->plotArea().center().x());
            break;
        case Qt::Key_Left:
            chart()->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            chart()->scroll(10, 0);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, 10);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, -10);
            break;
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

template<class T>
void QPlot<T>::zoomW(qreal factor, qreal xPos)
{
    QRectF rect = chart()->plotArea();
    rect.setWidth(factor * rect.width());

    QRectF plotarea = chart()->plotArea();

    if (factor < 1.0)
    {
        chart()->scroll((xPos - plotarea.left())*factor, 0);
    }

    chart()->zoomIn(rect);

    if (factor > 1.0)
    {
        chart()->scroll((xPos - plotarea.left())*(-(1/factor)), 0);
    }

    if ((currentZoomMinTime < allMinTime) || (allMaxTime < currentZoomMaxTime))
    {
        currentZoomPlot = nullptr;
    }

    if (currentZoomPlot == nullptr)
    {
        currentZoomMinTime = allMinTime;
        currentZoomMaxTime = allMaxTime;
    }
    if (currentZoomPlot == this)
    {
        if (QDateTimeAxis* axis = dynamic_cast<QDateTimeAxis*>(chart()->axisX()))
        {
            currentZoomMinTime = axis->min();
            currentZoomMaxTime = axis->max();
        }
    }
    else
    {
        if (QDateTimeAxis* axis = dynamic_cast<QDateTimeAxis*>(chart()->axisX()))
        {
            chart()->zoomReset();
            axis->setMin(currentZoomMinTime);
            axis->setMax(currentZoomMaxTime);
        }
    }
}
