#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDoubleSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <vector>

#include "drawwidget.h"
#include "helpers.h"
#include "histogramwidget.h"
#include "matrix.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    DrawWidget* draw = nullptr;
    HistogramWidget* histogram = nullptr;
    QTabWidget* tabs = nullptr;

    QPushButton* btnGenerate = nullptr;
    QPushButton* btnLoad = nullptr;
    QPushButton* btnApplyTransform = nullptr;
    QPushButton* btnResetTransform = nullptr;
    QPushButton* btnFilter = nullptr;

    QDoubleSpinBox* spinTx = nullptr;
    QDoubleSpinBox* spinTy = nullptr;
    QDoubleSpinBox* spinTz = nullptr;
    QDoubleSpinBox* spinRx = nullptr;
    QDoubleSpinBox* spinRy = nullptr;
    QDoubleSpinBox* spinRz = nullptr;

    QSpinBox* spinMeanK = nullptr;
    QDoubleSpinBox* spinStddevMul = nullptr;
    QSpinBox* spinHistBins = nullptr;

    QTableWidget* table = nullptr;
    QLabel* labelPointCount = nullptr;
    QLabel* labelInlierCount = nullptr;
    QLabel* labelTimeSeq = nullptr;
    QLabel* labelTimePar = nullptr;
    QLabel* labelSpeedup = nullptr;

    std::vector<Point> originalPoints;
    std::vector<Point> points;
    std::vector<double> lastMeanDist;
    Matrix4x4 cumulativeMatrix;

    bool sorApplied = false;

    void generateCloud();
    void loadCloud();
    void applyTransform();
    void resetTransform();
    void applyFilter();
    void refreshViews();
    void updateStatistics();
    void fillTableRow(int row, const AxisStatistics& stats);
};

#endif
