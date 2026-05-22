#include "histogramwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QString>
#include <algorithm>
#include <cmath>

HistogramWidget::HistogramWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(700, 420);
}

void HistogramWidget::setPoints(const std::vector<Point>& p, int bins)
{
    points = p;
    binCount = std::max(4, bins);
    update();
}

void HistogramWidget::setBins(int bins)
{
    binCount = std::max(4, bins);
    update();
}

void HistogramWidget::drawAxisHistogram(QPainter& painter,
                                        const QRect& area,
                                        const QString& title,
                                        int axis) const
{
    painter.setPen(Qt::white);
    painter.drawText(area.topLeft() + QPoint(4, 14), title);

    std::vector<double> values;
    values.reserve(points.size());
    for (const auto& p : points) {
        if (p.removed)
            continue;
        if (axis == 0)
            values.push_back(p.x);
        else if (axis == 1)
            values.push_back(p.y);
        else
            values.push_back(p.z);
    }

    if (values.empty()) {
        painter.drawText(area.center(), QStringLiteral("Нет данных"));
        return;
    }

    const double vmin = *std::min_element(values.begin(), values.end());
    const double vmax = *std::max_element(values.begin(), values.end());
    const double range = vmax - vmin;
    if (range < 1e-12)
        return;

    std::vector<int> hist(binCount, 0);
    for (double v : values) {
        int b = static_cast<int>((v - vmin) / range * (binCount - 1));
        b = std::clamp(b, 0, binCount - 1);
        ++hist[b];
    }

    const int maxCount = *std::max_element(hist.begin(), hist.end());
    const QRect plot = area.adjusted(28, 22, -8, -24);

    // Рамка без заливки (иначе после X остаётся синий brush и Y/Z заливаются целиком).
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(120, 120, 120));
    painter.drawRect(plot);

    const double barW = static_cast<double>(plot.width()) / binCount;
    painter.setBrush(QColor(80, 160, 255));
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < binCount; ++i) {
        if (hist[i] <= 0)
            continue;
        const double barH = maxCount > 0
            ? static_cast<double>(hist[i]) / maxCount * plot.height()
            : 0.0;
        const int x = plot.left() + static_cast<int>(i * barW);
        const int barWidth = std::max(1, static_cast<int>(barW) - 2);
        painter.drawRect(x, plot.bottom() - static_cast<int>(barH), barWidth,
                         static_cast<int>(barH));
    }

    painter.setPen(Qt::lightGray);
    painter.setFont(QFont(QStringLiteral("Arial"), 8));
    painter.drawText(plot.bottomLeft() + QPoint(0, 18),
                     QStringLiteral("min=%1 max=%2").arg(vmin, 0, 'f', 2).arg(vmax, 0, 'f', 2));
}

void HistogramWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));

    const int w = width();
    const int h = height();
    if (w < 20 || h < 20)
        return;
    const int third = h / 3;

    drawAxisHistogram(painter, QRect(0, 0, w, third), QStringLiteral("X"), 0);
    drawAxisHistogram(painter, QRect(0, third, w, third), QStringLiteral("Y"), 1);
    drawAxisHistogram(painter, QRect(0, 2 * third, w, h - 2 * third), QStringLiteral("Z"), 2);
}
