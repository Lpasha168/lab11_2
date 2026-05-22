#ifndef SORFILTER_H
#define SORFILTER_H

#include "helpers.h"
#include <chrono>
#include <vector>

struct SorParams
{
    int meanK = 20;
    double stddevMul = 1.0;
};

struct SorTiming
{
    double sequentialMs = 0.0;
    double parallelMs = 0.0;
    double speedup = 1.0;
};

enum class SorMode { Sequential, Parallel };

/// Среднее расстояние до K ближайших соседей для одной точки.
double meanKNeighborDistance(const std::vector<Point>& points, size_t index, int k);

/// Вычисляет meanDist для всех точек (seq или par).
std::vector<double> computeMeanDistances(const std::vector<Point>& points,
                                         const SorParams& params,
                                         SorMode mode);

/// Порог и пометка выбросов; возвращает глобальные mean/std порога.
struct SorThreshold
{
    double globalMean = 0.0;
    double globalStdDev = 0.0;
    double threshold = 0.0;
};

SorThreshold applySorMarking(std::vector<Point>& points,
                             const std::vector<double>& meanDist,
                             const SorParams& params);

/// Полный цикл SOR с замером времени seq/par (для speedup в GUI).
SorTiming runSorWithTiming(std::vector<Point>& points,
                           const SorParams& params,
                           std::vector<double>& meanDistOut);

#endif
