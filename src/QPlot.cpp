#include "QPlot.h"
#include <QMouseEvent>
#include <QValueAxis>
#include <QLineSeries>

QPlot::QPlot(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    lastMouseXPos = 0.0;
    timerId = startTimer(1000);

    QLineSeries* m_series = dynamic_cast<QLineSeries*>(chart->series().at(0));

    connect(m_series, &QLineSeries::pointAdded, [=](int index){
        qreal y = m_series->at(index).y();
        qreal x = m_series->at(index).x();

        qreal yMax = ((QValueAxis*)chart->axisY())->max();
        qreal yMin = ((QValueAxis*)chart->axisY())->min();
        if ( (y < yMin) || (y > yMax) )
        {
            if (y < yMin)
                yMin = y;
            if (y > yMax)
                yMax = y;
            chart->axisY()->setRange(yMin, yMax);
        }

        qreal xMax = ((QValueAxis*)chart->axisX())->max();
        qreal xMin = ((QValueAxis*)chart->axisX())->min();
        if ( (x < xMin) || (x > xMax) )
        {
            if (x < xMin)
                xMin = x;
            if (x > xMax)
                xMax = x;
            chart->axisX()->setRange(xMin, xMax);
        }

    });
}

void QPlot::timerEvent(QTimerEvent*)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        if (series->points().count() > 0)
        {
            qreal xMin = series->points().at(0).x();
            qreal xMax = series->points().at(0).x();

            QList<QPointF> points = series->points();
            QList<QPointF>::iterator it;
            for (it = points.begin(); it != points.end(); ++it)
            {
                if ((*it).x() < xMin)
                    xMin = (*it).x();
                if ((*it).x() > xMax)
                    xMax = (*it).x();
            }

            if (QValueAxis* axis = dynamic_cast<QValueAxis*>(chart()->axisX()))
            {
                if ( (axis->min() < xMin) || (axis->max() > xMax) )
                {
                    chart()->zoomReset();
                    chart()->axisX()->setRange(xMin, xMax);
                }
            }
        }
    }
}

void QPlot::mouseMoveEvent(QMouseEvent *event)
{
    lastMouseXPos = event->localPos().x();
    QChartView::mouseMoveEvent(event);
}

void QPlot::wheelEvent(QWheelEvent *event)
{
    qreal factor=1.0;
    factor *= event->angleDelta().y() > 0 ? 0.5 : 2;
    zoomW(factor, lastMouseXPos);
}

void QPlot::keyPressEvent(QKeyEvent *event)
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

void QPlot::zoomW(qreal factor, qreal xPos)
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
}
