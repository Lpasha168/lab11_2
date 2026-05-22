#include "mainwindow.h"
#include "io_xyz.h"
#include "sorfilter.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QVBoxLayout>

namespace {

QString formatStat(double v, bool valid)
{
    return valid ? QString::number(v, 'f', 4) : QStringLiteral("—");
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Облако точек — вариант 3"));
    resize(1200, 800);

    QWidget* central = new QWidget;
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);

    tabs = new QTabWidget;
    draw = new DrawWidget;
    draw->setMinimumSize(640, 480);
    histogram = new HistogramWidget;

    auto* cloudPage = new QWidget;
    auto* cloudLayout = new QVBoxLayout(cloudPage);
    cloudLayout->addWidget(draw);
    cloudLayout->setContentsMargins(0, 0, 0, 0);

    tabs->addTab(cloudPage, QStringLiteral("Облако"));
    tabs->addTab(histogram, QStringLiteral("Гистограммы"));

    auto* rightPanel = new QVBoxLayout;

    auto* fileBox = new QGroupBox(QStringLiteral("Данные"));
    auto* fileLayout = new QVBoxLayout(fileBox);
    btnGenerate = new QPushButton(QStringLiteral("Сгенерировать с шумом"));
    btnLoad = new QPushButton(QStringLiteral("Загрузить XYZ"));
    fileLayout->addWidget(btnGenerate);
    fileLayout->addWidget(btnLoad);

    auto* transformBox = new QGroupBox(QStringLiteral("Преобразования 4×4"));
    auto* transformGrid = new QGridLayout(transformBox);

    spinTx = new QDoubleSpinBox;
    spinTy = new QDoubleSpinBox;
    spinTz = new QDoubleSpinBox;
    spinRx = new QDoubleSpinBox;
    spinRy = new QDoubleSpinBox;
    spinRz = new QDoubleSpinBox;

    for (auto* s : {spinTx, spinTy, spinTz}) {
        s->setRange(-500, 500);
        s->setDecimals(2);
        s->setSingleStep(5);
    }
    for (auto* s : {spinRx, spinRy, spinRz}) {
        s->setRange(-360, 360);
        s->setDecimals(1);
        s->setSuffix(QStringLiteral("°"));
    }

    transformGrid->addWidget(new QLabel(QStringLiteral("Tx")), 0, 0);
    transformGrid->addWidget(spinTx, 0, 1);
    transformGrid->addWidget(new QLabel(QStringLiteral("Ty")), 1, 0);
    transformGrid->addWidget(spinTy, 1, 1);
    transformGrid->addWidget(new QLabel(QStringLiteral("Tz")), 2, 0);
    transformGrid->addWidget(spinTz, 2, 1);
    transformGrid->addWidget(new QLabel(QStringLiteral("Rx")), 0, 2);
    transformGrid->addWidget(spinRx, 0, 3);
    transformGrid->addWidget(new QLabel(QStringLiteral("Ry")), 1, 2);
    transformGrid->addWidget(spinRy, 1, 3);
    transformGrid->addWidget(new QLabel(QStringLiteral("Rz")), 2, 2);
    transformGrid->addWidget(spinRz, 2, 3);

    btnApplyTransform = new QPushButton(QStringLiteral("Применить (T·Rz·Ry·Rx)"));
    btnResetTransform = new QPushButton(QStringLiteral("Сбросить преобразование"));
    transformGrid->addWidget(btnApplyTransform, 3, 0, 1, 2);
    transformGrid->addWidget(btnResetTransform, 3, 2, 1, 2);

    auto* sorBox = new QGroupBox(QStringLiteral("SOR-фильтр"));
    auto* sorLayout = new QGridLayout(sorBox);

    spinMeanK = new QSpinBox;
    spinMeanK->setRange(5, 100);
    spinMeanK->setValue(20);

    spinStddevMul = new QDoubleSpinBox;
    spinStddevMul->setRange(0.1, 10.0);
    spinStddevMul->setSingleStep(0.1);
    spinStddevMul->setValue(1.0);

    btnFilter = new QPushButton(QStringLiteral("Применить SOR"));

    sorLayout->addWidget(new QLabel(QStringLiteral("Mean K")), 0, 0);
    sorLayout->addWidget(spinMeanK, 0, 1);
    sorLayout->addWidget(new QLabel(QStringLiteral("Stddev Mul")), 1, 0);
    sorLayout->addWidget(spinStddevMul, 1, 1);
    sorLayout->addWidget(btnFilter, 2, 0, 1, 2);

    labelTimeSeq = new QLabel(QStringLiteral("Последовательно: —"));
    labelTimePar = new QLabel(QStringLiteral("Параллельно: —"));
    labelSpeedup = new QLabel(QStringLiteral("Speedup: —"));
    sorLayout->addWidget(labelTimeSeq, 3, 0, 1, 2);
    sorLayout->addWidget(labelTimePar, 4, 0, 1, 2);
    sorLayout->addWidget(labelSpeedup, 5, 0, 1, 2);

    auto* histBox = new QGroupBox(QStringLiteral("Гистограммы"));
    auto* histLayout = new QHBoxLayout(histBox);
    spinHistBins = new QSpinBox;
    spinHistBins->setRange(8, 128);
    histLayout->addWidget(new QLabel(QStringLiteral("Бинов:")));
    histLayout->addWidget(spinHistBins);

    labelPointCount = new QLabel(QStringLiteral("Точек: 0"));
    labelInlierCount = new QLabel(QStringLiteral("Inliers: 0"));

    table = new QTableWidget(4, 4);
    const QStringList headers = {QStringLiteral("Min"),
                               QStringLiteral("Max"),
                               QStringLiteral("Mean"),
                               QStringLiteral("StdDev")};
    const QStringList rows = {QStringLiteral("X"),
                              QStringLiteral("Y"),
                              QStringLiteral("Z"),
                              QStringLiteral("Mean K dist")};
    table->setHorizontalHeaderLabels(headers);
    for (int r = 0; r < rows.size(); ++r)
        table->setVerticalHeaderItem(r, new QTableWidgetItem(rows[r]));

    rightPanel->addWidget(fileBox);
    rightPanel->addWidget(transformBox);
    rightPanel->addWidget(sorBox);
    rightPanel->addWidget(histBox);
    rightPanel->addWidget(labelPointCount);
    rightPanel->addWidget(labelInlierCount);
    rightPanel->addWidget(new QLabel(QStringLiteral("Статистика (inliers после SOR):")));
    rightPanel->addWidget(table);
    rightPanel->addStretch();

    mainLayout->addWidget(tabs, 3);
    mainLayout->addLayout(rightPanel, 2);

    cumulativeMatrix = Matrix4x4::identity();

    connect(btnGenerate, &QPushButton::clicked, this, &MainWindow::generateCloud);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadCloud);
    connect(btnApplyTransform, &QPushButton::clicked, this, &MainWindow::applyTransform);
    connect(btnResetTransform, &QPushButton::clicked, this, &MainWindow::resetTransform);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::applyFilter);
    spinHistBins->blockSignals(true);
    spinHistBins->setValue(32);
    spinHistBins->blockSignals(false);

    connect(spinHistBins, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int bins) {
        histogram->setBins(bins);
        histogram->setPoints(points, bins);
    });

    updateStatistics();
}

void MainWindow::generateCloud()
{
    points.clear();
    const int coreCount = 2000;
    const int noiseCount = 150;

    for (int i = 0; i < coreCount; ++i) {
        points.push_back({QRandomGenerator::global()->generateDouble() * 200.0 - 100.0,
                          QRandomGenerator::global()->generateDouble() * 200.0 - 100.0,
                          QRandomGenerator::global()->generateDouble() * 200.0 - 100.0,
                          false});
    }
    for (int i = 0; i < noiseCount; ++i) {
        points.push_back({QRandomGenerator::global()->generateDouble() * 400.0 - 200.0,
                          QRandomGenerator::global()->generateDouble() * 400.0 - 200.0,
                          QRandomGenerator::global()->generateDouble() * 400.0 - 200.0,
                          false});
    }

    originalPoints = points;
    cumulativeMatrix = Matrix4x4::identity();
    sorApplied = false;
    lastMeanDist.clear();
    refreshViews();
}

void MainWindow::loadCloud()
{
    const QString file = QFileDialog::getOpenFileName(
        this, QStringLiteral("Открыть XYZ"), QString(), QStringLiteral("XYZ (*.xyz *.txt);;All (*)"));
    if (file.isEmpty())
        return;

    points = loadXYZ(file);
    originalPoints = points;
    cumulativeMatrix = Matrix4x4::identity();
    sorApplied = false;
    lastMeanDist.clear();
    refreshViews();
}

void MainWindow::applyTransform()
{
    if (originalPoints.empty())
        return;

    const double deg2rad = 3.14159265358979323846 / 180.0;
    const Matrix4x4 t = Matrix4x4::translation(spinTx->value(), spinTy->value(), spinTz->value());
    const Matrix4x4 rx = Matrix4x4::rotationX(spinRx->value() * deg2rad);
    const Matrix4x4 ry = Matrix4x4::rotationY(spinRy->value() * deg2rad);
    const Matrix4x4 rz = Matrix4x4::rotationZ(spinRz->value() * deg2rad);

    const Matrix4x4 step = t * rz * ry * rx;
    cumulativeMatrix *= step;

    points = originalPoints;
    for (auto& p : points)
        p.removed = false;
    cumulativeMatrix.transformCloud(points);

    sorApplied = false;
    lastMeanDist.clear();
    refreshViews();
}

void MainWindow::resetTransform()
{
    cumulativeMatrix = Matrix4x4::identity();
    spinTx->setValue(0);
    spinTy->setValue(0);
    spinTz->setValue(0);
    spinRx->setValue(0);
    spinRy->setValue(0);
    spinRz->setValue(0);

    points = originalPoints;
    for (auto& p : points)
        p.removed = false;

    sorApplied = false;
    lastMeanDist.clear();
    refreshViews();
}

void MainWindow::applyFilter()
{
    if (originalPoints.empty())
        return;

    points = originalPoints;
    cumulativeMatrix.transformCloud(points);
    for (auto& p : points)
        p.removed = false;

    SorParams params;
    params.meanK = spinMeanK->value();
    params.stddevMul = spinStddevMul->value();

    const SorTiming timing = runSorWithTiming(points, params, lastMeanDist);
    sorApplied = true;

    labelTimeSeq->setText(
        QStringLiteral("Последовательно: %1 мс").arg(timing.sequentialMs, 0, 'f', 2));
    labelTimePar->setText(
        QStringLiteral("Параллельно: %1 мс").arg(timing.parallelMs, 0, 'f', 2));
    labelSpeedup->setText(
        QStringLiteral("Speedup: %1×").arg(timing.speedup, 0, 'f', 2));

    refreshViews();
}

void MainWindow::refreshViews()
{
    draw->setUseFilterColors(sorApplied);
    draw->setPoints(points);
    histogram->setPoints(points, spinHistBins->value());
    updateStatistics();
}

void MainWindow::fillTableRow(int row, const AxisStatistics& stats)
{
    auto set = [this, row](int col, const QString& text) {
        table->setItem(row, col, new QTableWidgetItem(text));
    };
    set(0, formatStat(stats.minV, stats.valid));
    set(1, formatStat(stats.maxV, stats.valid));
    set(2, formatStat(stats.mean, stats.valid));
    set(3, formatStat(stats.stdDev, stats.valid));
}

void MainWindow::updateStatistics()
{
    const int total = static_cast<int>(points.size());
    int inliers = 0;
    for (const auto& p : points) {
        if (!p.removed)
            ++inliers;
    }

    labelPointCount->setText(QStringLiteral("Точек: %1").arg(total));
    labelInlierCount->setText(
        sorApplied
            ? QStringLiteral("Inliers: %1 | Outliers: %2").arg(inliers).arg(total - inliers)
            : QStringLiteral("Inliers: %1 (до фильтра)").arg(total));

    const bool inliersOnly = sorApplied;

    fillTableRow(0, calcAxisStatistics(points, 0, inliersOnly));
    fillTableRow(1, calcAxisStatistics(points, 1, inliersOnly));
    fillTableRow(2, calcAxisStatistics(points, 2, inliersOnly));

    if (!lastMeanDist.empty())
        fillTableRow(3, calcValueStatistics(lastMeanDist, inliersOnly, &points));
    else
        fillTableRow(3, AxisStatistics{});
}
