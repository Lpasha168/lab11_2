
#include "io_xyz.h"
#include <fstream>

std::vector<Point> loadXYZ(const QString& path)
{
    std::vector<Point> p;
    std::ifstream f(path.toStdString());

    double x,y,z;
    while(f>>x>>y>>z)
        p.push_back({x,y,z,false});

    return p;
}
