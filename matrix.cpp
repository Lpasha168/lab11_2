#include "matrix.h"
#include <cmath>

Matrix4x4::Matrix4x4()
{
    m.fill(0.0);
    at(0, 0) = 1.0;
    at(1, 1) = 1.0;
    at(2, 2) = 1.0;
    at(3, 3) = 1.0;
}

Matrix4x4 Matrix4x4::identity()
{
    return Matrix4x4();
}

Matrix4x4 Matrix4x4::translation(double tx, double ty, double tz)
{
    Matrix4x4 r;
    r.at(0, 3) = tx;
    r.at(1, 3) = ty;
    r.at(2, 3) = tz;
    return r;
}

Matrix4x4 Matrix4x4::rotationX(double radians)
{
    Matrix4x4 r;
    const double c = std::cos(radians);
    const double s = std::sin(radians);
    r.at(1, 1) = c;
    r.at(1, 2) = -s;
    r.at(2, 1) = s;
    r.at(2, 2) = c;
    return r;
}

Matrix4x4 Matrix4x4::rotationY(double radians)
{
    Matrix4x4 r;
    const double c = std::cos(radians);
    const double s = std::sin(radians);
    r.at(0, 0) = c;
    r.at(0, 2) = s;
    r.at(2, 0) = -s;
    r.at(2, 2) = c;
    return r;
}

Matrix4x4 Matrix4x4::rotationZ(double radians)
{
    Matrix4x4 r;
    const double c = std::cos(radians);
    const double s = std::sin(radians);
    r.at(0, 0) = c;
    r.at(0, 1) = -s;
    r.at(1, 0) = s;
    r.at(1, 1) = c;
    return r;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
    Matrix4x4 r;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            double sum = 0.0;
            for (int k = 0; k < 4; ++k)
                sum += at(row, k) * other.at(k, col);
            r.at(row, col) = sum;
        }
    }
    return r;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
{
    *this = *this * other;
    return *this;
}

Point Matrix4x4::transformPoint(const Point& p) const
{
    const double x = at(0, 0) * p.x + at(0, 1) * p.y + at(0, 2) * p.z + at(0, 3);
    const double y = at(1, 0) * p.x + at(1, 1) * p.y + at(1, 2) * p.z + at(1, 3);
    const double z = at(2, 0) * p.x + at(2, 1) * p.y + at(2, 2) * p.z + at(2, 3);
    const double w = at(3, 0) * p.x + at(3, 1) * p.y + at(3, 2) * p.z + at(3, 3);
    if (std::abs(w) < 1e-12)
        return {x, y, z, p.removed};
    return {x / w, y / w, z / w, p.removed};
}

void Matrix4x4::transformCloud(std::vector<Point>& cloud) const
{
    for (auto& p : cloud)
        p = transformPoint(p);
}
