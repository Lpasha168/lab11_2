#include "helpers.h"

#include <algorithm>
#include <cmath>
#include <numeric>

AxisStatistics calcAxisStatistics(const std::vector<Point>& points,
                                  int mode,
                                  bool inliersOnly)
{
    std::vector<double> values;
    values.reserve(points.size());

    for (const auto& p : points) {
        if (inliersOnly && p.removed)
            continue;
        if (mode == 0)
            values.push_back(p.x);
        else if (mode == 1)
            values.push_back(p.y);
        else
            values.push_back(p.z);
    }

    return calcValueStatistics(values);
}

AxisStatistics calcValueStatistics(const std::vector<double>& values,
                                   bool inliersMask,
                                   const std::vector<Point>* points)
{
    AxisStatistics stats;
    std::vector<double> filtered;
    filtered.reserve(values.size());

    for (size_t i = 0; i < values.size(); ++i) {
        if (inliersMask && points && i < points->size() && (*points)[i].removed)
            continue;
        filtered.push_back(values[i]);
    }

    if (filtered.empty())
        return stats;

    stats.valid = true;
    stats.count = static_cast<int>(filtered.size());
    stats.minV = *std::min_element(filtered.begin(), filtered.end());
    stats.maxV = *std::max_element(filtered.begin(), filtered.end());

    const double sum = std::accumulate(filtered.begin(), filtered.end(), 0.0);
    stats.mean = sum / filtered.size();

    double sq = 0.0;
    for (double v : filtered) {
        const double d = v - stats.mean;
        sq += d * d;
    }
    stats.stdDev = std::sqrt(sq / filtered.size());
    return stats;
}
