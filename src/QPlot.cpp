#include "QPlot.h"
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>
#include <QDateTimeAxis>

QPlot::QPlot() :
    layout(new QBoxLayout(QBoxLayout::Direction::TopToBottom, this)),
    timerId(startTimer(200)),
    allMinTime(QDateTime::currentDateTime()),
    allMaxTime(QDateTime::currentDateTime()),
    currentZoomMinTime(QDateTime::currentDateTime()),
    currentZoomMaxTime(QDateTime::currentDateTime()),
    currentZoomPlot(nullptr)
{
    QGroupBox *controlGroupBox = new QGroupBox(tr("Control"));
    auto controlLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, controlGroupBox);

    QGroupBox *visibleGroupBox = new QGroupBox(tr("Plots"));
    visibleLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, visibleGroupBox);
    controlLayout->addWidget(visibleGroupBox);

    auto recordCheckBox = new QCheckBox("Record");
    recordCheckBox->setChecked(false);
    controlLayout->addWidget(recordCheckBox);
    connect(recordCheckBox, &QCheckBox::clicked, [=](bool value)
    {
        qDebug() << "Record " << (value ? "enabled" : "disabled");
        for (auto plot : plotViews)
        {
            plot->setRecordOnAppend(value);
        }
    });

    auto UpdateRecordCheckBox = new QCheckBox("Show Record");
    UpdateRecordCheckBox->setChecked(false);
    controlLayout->addWidget(UpdateRecordCheckBox);
    connect(UpdateRecordCheckBox, &QCheckBox::clicked, [=](bool value)
    {
        qDebug() << "Update Record " << (value ? "enabled" : "disabled");
        for (auto plot : plotViews)
        {
            plot->setUpdateOnAppend(value);
        }
    });

    auto unzoomButton = new QPushButton("Unzoom");
    controlLayout->addWidget(unzoomButton);
    connect(unzoomButton, &QPushButton::clicked, [=]()
    {
        qDebug() << "Unzoom ...";
        currentZoomPlot = nullptr;
    });

    auto exportButton = new QPushButton("Export .csv");
    controlLayout->addWidget(exportButton);
    connect(exportButton, &QPushButton::clicked, [=]()
    {
        qDebug() << "Export .csv TODO";
    });

    layout->addWidget(controlGroupBox);
    QGroupBox *plotsGroupBox = new QGroupBox(tr("Plots"));
    plotsLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, plotsGroupBox);
    layout->addWidget(plotsGroupBox);
}

QPlotView *QPlot::addView(std::string title, QPlotView::Type type, std::vector<std::string>* custom)
{
    QPlotView *plot = nullptr;
    switch (type)
    {
        case QPlotView::Type::BOOL:
        {
            plot = new QPlotView(title, QPlotView::Type::BOOL, currentZoomPlot, false);
            //plot->setRenderHint(QPainter::Antialiasing);
            break;
        }
        case QPlotView::Type::INTEGER:
        {
            plot = new QPlotView(title, QPlotView::Type::INTEGER, currentZoomPlot, false);
            //plot->setRenderHint(QPainter::Antialiasing);
            break;
        }
        case QPlotView::Type::DOUBLE:
        {
            plot = new QPlotView(title, QPlotView::Type::DOUBLE, currentZoomPlot, false);
            //plot->setRenderHint(QPainter::Antialiasing);
            break;
        }
        case QPlotView::Type::CUSTOM_CATEGORY:
        {
            plot = new QPlotView(title, QPlotView::Type::CUSTOM_CATEGORY, currentZoomPlot, false, custom);
            //plot->setRenderHint(QPainter::Antialiasing);
            break;
        }
        default:
        break;
    }
    if (plot != nullptr)
    {
        auto visibleCheckBox = new QCheckBox(title.c_str());
        visibleCheckBox->setChecked(true);
        visibleLayout->addWidget(visibleCheckBox);
        connect(visibleCheckBox, &QCheckBox::clicked, [=](bool value)
        {
            qDebug() << "visibleCheckBox " << (value ? "enabled" : "disabled");
            if (value)
                plot->show();
            else
                plot->hide();
        });
        plotViews.push_back(plot);
        plotsLayout->addWidget(plot);
    }
    return plot;
}

void QPlot::timerEvent(QTimerEvent*)
{
    //qDebug() << "timerEvent";

    // Update all Min/Max
    for (auto plot : plotViews)
    {
        if (plot->getMinTime() < allMinTime)
            allMinTime = plot->getMinTime();

        if (plot->getMaxTime() > allMaxTime)
            allMaxTime = plot->getMaxTime();
    }

    // Reset zoom plot, if unzoom is beyond the limits
    if ((currentZoomMinTime < allMinTime) || (allMaxTime < currentZoomMaxTime))
    {
        currentZoomPlot = nullptr;
    }
    // Make zoom invalid
    if (currentZoomPlot == nullptr)
    {
        currentZoomMinTime = allMinTime;
        currentZoomMaxTime = allMaxTime;
    }
    // Get the zoom window of current zoom plot view
    if (currentZoomPlot != nullptr)
    {
        if (QDateTimeAxis* axis = dynamic_cast<QDateTimeAxis*>(currentZoomPlot->chart()->axisX()))
        {
            currentZoomMinTime = axis->min();
            currentZoomMaxTime = axis->max();
        }
    }

    // Update all other views with zoom window
    for (auto plot : plotViews)
    {
        if ((currentZoomPlot != plot) && (plot->isVisible()))
        {
            if (QDateTimeAxis* axis = dynamic_cast<QDateTimeAxis*>(plot->chart()->axisX()))
            {
                plot->chart()->zoomReset();
                axis->setMin(currentZoomMinTime);
                axis->setMax(currentZoomMaxTime);
            }
        }
    }
}
