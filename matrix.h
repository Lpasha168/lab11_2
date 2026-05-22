#ifndef MATRIX_H
#define MATRIX_H

#include "helpers.h"
#include <array>
#include <vector>

/// Матрица 4×4 для однородных преобразований (ЛР №10).
/// Хранение по строкам: m[row][col], умножение M * v (вектор-столбец).
class Matrix4x4
{
public:
    Matrix4x4();

    static Matrix4x4 identity();
    static Matrix4x4 translation(double tx, double ty, double tz);
    static Matrix4x4 rotationX(double radians);
    static Matrix4x4 rotationY(double radians);
    static Matrix4x4 rotationZ(double radians);

    Matrix4x4 operator*(const Matrix4x4& other) const;
    Matrix4x4& operator*=(const Matrix4x4& other);

    Point transformPoint(const Point& p) const;
    void transformCloud(std::vector<Point>& cloud) const;

    double at(int row, int col) const { return m[row * 4 + col]; }
    double& at(int row, int col) { return m[row * 4 + col]; }

private:
    std::array<double, 16> m{};
};

#endif
