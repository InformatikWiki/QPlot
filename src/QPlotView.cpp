#include "QPlotView.h"
#include <QMouseEvent>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QLineSeries>
#include <QDebug>

QPlotView::QPlotView(std::string title, Type type, QPlotView*& currentZoomPlot, bool updateOnAppend, std::vector<std::string>* custom, QWidget *parent) :
    QChartView(new QChart(), parent),
    title(title),
    type(type),
    currentZoomPlot(currentZoomPlot),
    recordOnAppend(false),
    updateOnAppend(updateOnAppend),
    lastMouseXPos(0.0),
    minTime(QDateTime::currentDateTime()),
    maxTime(QDateTime::currentDateTime())
{
    QLineSeries *series = new QLineSeries();
    chart()->addSeries(series);
    chart()->setTitle(title.c_str());
    //chart()->setAnimationOptions(QChart::SeriesAnimations);
    chart()->legend()->hide();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm:ss.zzz");
    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->axisX()->setMin(minTime);
    series->attachAxis(axisX);

    if (type == Type::BOOL)
    {
        QCategoryAxis *axisY = new QCategoryAxis();
        axisY->append("False", 0.2);
        axisY->append("-", 0.8);
        axisY->append("True", 1);
        axisY->setRange(0, 1);
        chart()->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }
    else if (type == Type::CUSTOM_CATEGORY)
    {
        QCategoryAxis *axisY = new QCategoryAxis();
        for (unsigned int i = 0; i < custom->size(); i++)
        {
            axisY->append(custom->at(i).c_str(), i+1);
        }
        axisY->setRange(0, custom->size());
        chart()->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }
    else
    {
        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%.0f");
        axisY->applyNiceNumbers();
        chart()->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }

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

    //qDebug() << "tickAnchor " << ((QValueAxis*)axisY)->tickAnchor(); This property was introduced in Qt 5.12.
    //qDebug() << "Margins " << chart()->margins().left();

    //QRectF rect = chart()->plotArea();
    //qDebug() << "Left ..." << rect.left();

    connect(this, SIGNAL(signal_append(QDateTime, bool)),   this, SLOT(slot_append(QDateTime, bool)));
    connect(this, SIGNAL(signal_append(QDateTime, int)),    this, SLOT(slot_append(QDateTime, int)));
    connect(this, SIGNAL(signal_append(QDateTime, double)), this, SLOT(slot_append(QDateTime, double)));
}

QDateTime QPlotView::getMinTime(void)
{
    return minTime;
}

QDateTime QPlotView::getMaxTime(void)
{
    return maxTime;
}

void QPlotView::setRecordOnAppend(bool value)
{
    recordOnAppend = value;
}

void QPlotView::setUpdateOnAppend(bool value)
{
    if (!updateOnAppend && value)
    {
        while (!buffer.empty())
        {
            std::tuple<QDateTime, qreal> sample = buffer.at(0);
            buffer.erase(buffer.begin());

            QDateTime time = std::get<0>(sample);
            qreal value    = std::get<1>(sample);

            switch (type)
            {
                case BOOL:
                    slot_append(time, (bool)value);
                break;

                case INTEGER:
                    slot_append(time, (int)value);
                break;

                case DOUBLE:
                    slot_append(time, (double)value);
                break;

                case CUSTOM_CATEGORY:
                    slot_append(time, (int)value);
                break;

                default:
                break;
            }
        }
    }
    updateOnAppend = value;
}

void QPlotView::append(bool value)
{
    if (recordOnAppend)
    {
        if (type == BOOL)
        {
            if (!updateOnAppend)
            {
                buffer.push_back(std::make_tuple(QDateTime::currentDateTime(), (qreal)value));
            }
            else
            {
                emit signal_append(QDateTime::currentDateTime(), value);
            }
        }
    }
}

void QPlotView::slot_append(QDateTime dateTime, bool value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        qreal xValue = dateTime.toMSecsSinceEpoch();
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

        if (updateOnAppend)
        {
            chart()->axisX()->setMax(dateTime);
        }
        maxTime = dateTime;
    }
}

void QPlotView::append(int value)
{
    if (recordOnAppend)
    {
        if ((type == INTEGER) || (type == CUSTOM_CATEGORY))
        {
            if (!updateOnAppend)
            {
                buffer.push_back(std::make_tuple(QDateTime::currentDateTime(), (qreal)value));
            }
            else
            {
                emit signal_append(QDateTime::currentDateTime(), value);
            }
        }
    }
}

void QPlotView::slot_append(QDateTime dateTime, int value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
       qreal xValue = dateTime.toMSecsSinceEpoch();
       if (series->points().count() > 0)
       {
           QPointF last = series->points().last();
           series->append(QPointF(xValue, last.y()));
       }
       qreal qvalue = (qreal)value;
       if ((type == CUSTOM_CATEGORY))
           qvalue += 0.5;
       series->append(QPointF(xValue, qvalue));

       if (updateOnAppend)
       {
           chart()->axisX()->setMax(dateTime);
       }
       maxTime = dateTime;
    }
}

void QPlotView::append(double value)
{
    if (recordOnAppend)
    {
        if (type == DOUBLE)
        {
            if (!updateOnAppend)
            {
                buffer.push_back(std::make_tuple(QDateTime::currentDateTime(), (qreal)value));
            }
            else
            {
                emit signal_append(QDateTime::currentDateTime(), value);
            }
        }
    }
}

void QPlotView::slot_append(QDateTime dateTime, double value)
{
    if (QLineSeries* series = dynamic_cast<QLineSeries*>(chart()->series().at(0)))
    {
        qreal xValue = dateTime.toMSecsSinceEpoch();
        series->append(xValue, (qreal)value);

        if (updateOnAppend)
        {
            chart()->axisX()->setMax(dateTime);
        }
        maxTime = dateTime;
    }
}

void QPlotView::mouseMoveEvent(QMouseEvent *event)
{
    lastMouseXPos = event->localPos().x();
    QChartView::mouseMoveEvent(event);
}

void QPlotView::wheelEvent(QWheelEvent *event)
{
    currentZoomPlot = this;
    qreal factor = event->angleDelta().y() > 0 ? 0.5 : 2;
    zoomW(factor, lastMouseXPos);
}

void QPlotView::keyPressEvent(QKeyEvent *event)
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

void QPlotView::zoomW(qreal factor, qreal xPos)
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
