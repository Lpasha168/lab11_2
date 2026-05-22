#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include "helpers.h"
#include <QWidget>
#include <vector>

class QPainter;
class QPaintEvent;

/// Гистограммы распределения X, Y, Z (отрисовка через QPainter).
class HistogramWidget : public QWidget
{
public:
    explicit HistogramWidget(QWidget* parent = nullptr);

    void setPoints(const std::vector<Point>& points, int bins = 32);
    void setBins(int bins);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<Point> points;
    int binCount = 32;

    void drawAxisHistogram(QPainter& painter,
                           const QRect& area,
                           const QString& title,
                           int axis) const;
};

#endif
