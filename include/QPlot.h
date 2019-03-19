#ifndef QPLOTVIEW_H
#define QPLOTVIEW_H

#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <vector>
#include "QPlotView.h"

class QPlot : public QWidget
{
    Q_OBJECT

public:
    QPlot();

    //template <typename T>
    QPlotView* addView(std::string title, QPlotView::Type, std::vector<std::string>* custom = nullptr);

protected:
    void timerEvent(QTimerEvent *event);

private:
    QBoxLayout* layout;
    QBoxLayout* visibleLayout;
    QBoxLayout* plotsLayout;
    std::vector<QPlotView*> plotViews;
    int timerId;

    QDateTime allMinTime;
    QDateTime allMaxTime;
    QDateTime currentZoomMinTime;
    QDateTime currentZoomMaxTime;
    QPlotView* currentZoomPlot;
};

#endif // QPLOTVIEW_H
