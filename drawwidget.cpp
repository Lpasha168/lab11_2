#include "drawwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

DrawWidget::DrawWidget(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void DrawWidget::setPoints(const std::vector<Point>& p)
{
    points = p;
    update();
}

void DrawWidget::setUseFilterColors(bool enabled)
{
    filterColors = enabled;
    update();
}

void DrawWidget::mousePressEvent(QMouseEvent* event)
{
    lastMouse = event->pos();
}

void DrawWidget::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint delta = event->pos() - lastMouse;
    lastMouse = event->pos();

    angleY += delta.x() * 0.01;
    angleX += delta.y() * 0.01;

    update();
}

void DrawWidget::wheelEvent(QWheelEvent* event)
{
    scale += event->angleDelta().y() * 0.001;
    scale = std::clamp(scale, 0.5, 20.0);
    update();
}

void DrawWidget::drawPoint(QPainter& painter, const Point& p, int w, int h) const
{
    const double cx = std::cos(angleX);
    const double sx = std::sin(angleX);
    double y = p.y * cx - p.z * sx;
    double z = p.y * sx + p.z * cx;

    const double cy = std::cos(angleY);
    const double sy = std::sin(angleY);
    const double x = p.x * cy + z * sy;
    z = -p.x * sy + z * cy;

    const int px = static_cast<int>(x * scale + w / 2);
    const int py = static_cast<int>(y * scale + h / 2);

    if (filterColors) {
        if (p.removed)
            painter.setPen(QColor(255, 40, 40));
        else
            painter.setPen(QColor(220, 255, 220));
    } else {
        const int intensity = std::clamp(static_cast<int>((z + 100.0) * 1.5), 0, 255);
        painter.setPen(QColor(intensity, intensity, 255));
    }

    painter.drawEllipse(QPointF(px, py), 2, 2);
}

void DrawWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    for (const auto& p : points)
        drawPoint(painter, p, width(), height());
}
