#ifndef HELPERS_H
#define HELPERS_H

#include <vector>

struct Point
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    bool removed = false;
};

struct AxisStatistics
{
    double minV = 0.0;
    double maxV = 0.0;
    double mean = 0.0;
    double stdDev = 0.0;
    int count = 0;
    bool valid = false;
};

/// mode: 0=X, 1=Y, 2=Z; inliersOnly — только точки с removed==false.
AxisStatistics calcAxisStatistics(const std::vector<Point>& points,
                                  int mode,
                                  bool inliersOnly = false);

/// Статистика по массиву значений (например, средних расстояний до соседей).
AxisStatistics calcValueStatistics(const std::vector<double>& values,
                                   bool inliersMask = false,
                                   const std::vector<Point>* points = nullptr);

#endif
