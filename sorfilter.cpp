#include "sorfilter.h"

#include <algorithm>
#include <cmath>
#include <future>
#include <numeric>
#include <thread>
#include <vector>

namespace {

double elapsedMs(const std::chrono::steady_clock::time_point& t0,
                 const std::chrono::steady_clock::time_point& t1)
{
    using Ms = std::chrono::duration<double, std::milli>;
    return Ms(t1 - t0).count();
}

unsigned int workerCount()
{
    const unsigned int hw = std::thread::hardware_concurrency();
    return hw > 0 ? hw : 4u;
}

} // namespace

double meanKNeighborDistance(const std::vector<Point>& points, size_t index, int k)
{
    const size_t n = points.size();
    if (n <= 1 || k <= 0)
        return 0.0;

    std::vector<double> distances;
    distances.reserve(n - 1);

    for (size_t j = 0; j < n; ++j) {
        if (j == index)
            continue;
        const double dx = points[index].x - points[j].x;
        const double dy = points[index].y - points[j].y;
        const double dz = points[index].z - points[j].z;
        distances.push_back(std::sqrt(dx * dx + dy * dy + dz * dz));
    }

    const int kk = std::min(k, static_cast<int>(distances.size()));
    std::nth_element(distances.begin(), distances.begin() + kk, distances.end());

    double sum = 0.0;
    for (int t = 0; t < kk; ++t)
        sum += distances[t];
    return sum / kk;
}

std::vector<double> computeMeanDistances(const std::vector<Point>& points,
                                         const SorParams& params,
                                         SorMode mode)
{
    const size_t n = points.size();
    std::vector<double> meanDist(n, 0.0);
    if (n == 0 || params.meanK <= 0)
        return meanDist;

    if (mode == SorMode::Sequential) {
        for (size_t i = 0; i < n; ++i)
            meanDist[i] = meanKNeighborDistance(points, i, params.meanK);
        return meanDist;
    }

    const unsigned int workers = workerCount();
    std::vector<std::future<void>> tasks;
    tasks.reserve(workers);

    auto processRange = [&points, &meanDist, k = params.meanK](size_t begin, size_t end) {
        for (size_t i = begin; i < end; ++i)
            meanDist[i] = meanKNeighborDistance(points, i, k);
    };

    const size_t chunk = (n + workers - 1) / workers;
    for (unsigned int w = 0; w < workers; ++w) {
        const size_t begin = w * chunk;
        const size_t end = std::min(begin + chunk, n);
        if (begin >= end)
            break;
        tasks.push_back(std::async(std::launch::async, processRange, begin, end));
    }

    for (auto& task : tasks)
        task.get();

    return meanDist;
}

SorThreshold applySorMarking(std::vector<Point>& points,
                             const std::vector<double>& meanDist,
                             const SorParams& params)
{
    SorThreshold info;
    const size_t n = meanDist.size();
    if (n == 0)
        return info;

    double sum = std::accumulate(meanDist.begin(), meanDist.end(), 0.0);
    info.globalMean = sum / static_cast<double>(n);

    double sq = 0.0;
    for (double v : meanDist) {
        const double d = v - info.globalMean;
        sq += d * d;
    }
    info.globalStdDev = std::sqrt(sq / static_cast<double>(n));
    info.threshold = info.globalMean + params.stddevMul * info.globalStdDev;

    for (size_t i = 0; i < n; ++i)
        points[i].removed = meanDist[i] > info.threshold;

    return info;
}

SorTiming runSorWithTiming(std::vector<Point>& points,
                           const SorParams& params,
                           std::vector<double>& meanDistOut)
{
    SorTiming timing;
    const size_t n = points.size();
    if (n <= static_cast<size_t>(params.meanK))
        return timing;

    const auto tSeq0 = std::chrono::steady_clock::now();
    computeMeanDistances(points, params, SorMode::Sequential);
    const auto tSeq1 = std::chrono::steady_clock::now();

    const auto tPar0 = std::chrono::steady_clock::now();
    meanDistOut = computeMeanDistances(points, params, SorMode::Parallel);
    const auto tPar1 = std::chrono::steady_clock::now();

    timing.sequentialMs = elapsedMs(tSeq0, tSeq1);
    timing.parallelMs = elapsedMs(tPar0, tPar1);
    if (timing.parallelMs > 1e-9)
        timing.speedup = timing.sequentialMs / timing.parallelMs;
    else
        timing.speedup = 1.0;

    applySorMarking(points, meanDistOut, params);
    return timing;
}
