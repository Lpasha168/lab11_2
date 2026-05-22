#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include "helpers.h"
#include <QPoint>
#include <QWidget>

class QPainter;
class QPaintEvent;
class QMouseEvent;
class QWheelEvent;

class DrawWidget : public QWidget
{
public:
    explicit DrawWidget(QWidget* parent = nullptr);

    void setPoints(const std::vector<Point>& p);
    void setUseFilterColors(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    std::vector<Point> points;
    bool filterColors = false;

    double angleX = 0.0;
    double angleY = 0.0;
    double scale = 3.0;

    QPoint lastMouse;

    void drawPoint(QPainter& painter, const Point& p, int w, int h) const;
};

#endif
