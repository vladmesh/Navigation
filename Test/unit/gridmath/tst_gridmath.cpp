// Unit-тест GridMath (ADR 0001, Task 01). Только QtCore + QtTest.
#include <QtTest>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <vector>

#include "gridmath.h"

namespace {

// Ближайший double к m × 10^n, как у литерала "me n" в исходнике.
double literal(int mantissa, int exponent)
{
    char buffer[32];
    std::snprintf(buffer, sizeof buffer, "%de%d", mantissa, exponent);
    return std::strtod(buffer, nullptr);
}

// Шаг ровно равен одному из {1, 2, 5} × 10^n (без хвостов вида 199.99999).
bool isCanonicalStep(double step)
{
    if (!(step > 0.0) || !std::isfinite(step))
        return false;
    const int exponent = static_cast<int>(std::floor(std::log10(step)));
    for (int n = exponent - 1; n <= exponent + 1; ++n)
        for (int m : {1, 2, 5})
            if (step == literal(m, n))
                return true;
    return false;
}

// Предыдущее значение ряда 1-2-5 для канонического шага.
double previousCanonicalStep(double step)
{
    const int exponent = static_cast<int>(std::floor(std::log10(step)));
    for (int n = exponent - 1; n <= exponent + 1; ++n) {
        if (step == literal(1, n))
            return literal(5, n - 1);
        if (step == literal(2, n))
            return literal(1, n);
        if (step == literal(5, n))
            return literal(2, n);
    }
    return 0.0;
}

QByteArray describe(const std::vector<double> &values)
{
    QByteArray out = "{";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i)
            out += ", ";
        out += QByteArray::number(values[i], 'g', 17);
    }
    return out + "}";
}

} // namespace

// Точное сравнение double: QCOMPARE для double сравнивает нечётко (qFuzzyCompare).
#define COMPARE_EXACT(actual, expected)                                                    \
    do {                                                                                   \
        const double a_ = (actual);                                                        \
        const double e_ = (expected);                                                      \
        QVERIFY2(a_ == e_, QByteArray(#actual " = " + QByteArray::number(a_, 'g', 17)      \
                                      + ", expected " + QByteArray::number(e_, 'g', 17))   \
                               .constData());                                              \
    } while (0)

#define COMPARE_LINES(actual, expected)                                                    \
    do {                                                                                   \
        const std::vector<double> a_ = (actual);                                           \
        const std::vector<double> e_ = (expected);                                         \
        QVERIFY2(a_ == e_, QByteArray(#actual " = " + describe(a_) + ", expected "         \
                                      + describe(e_))                                      \
                               .constData());                                              \
    } while (0)

class TestGridMath : public QObject
{
    Q_OBJECT

private slots:
    void niceStep_examples_data();
    void niceStep_examples();
    void niceStep_invalidInput();
    void niceStep_maxLines();
    void niceStep_property();
    void gridLines_utmExample();
    void gridLines_boundaries();
    void gridLines_fractionalStep();
    void gridLines_invalidInput();
    void gridLines_maxCount();
    void labelDecimals_examples_data();
    void labelDecimals_examples();
};

void TestGridMath::niceStep_examples_data()
{
    QTest::addColumn<double>("span");
    QTest::addColumn<double>("expected");

    QTest::newRow("1000") << 1000.0 << 100.0;
    QTest::newRow("999") << 999.0 << 100.0;
    QTest::newRow("1001") << 1001.0 << 200.0;
    QTest::newRow("50") << 50.0 << 5.0;
    QTest::newRow("12345") << 12345.0 << 2000.0;
    QTest::newRow("2.5") << 2.5 << 0.5;
    QTest::newRow("7") << 7.0 << 1.0;
    QTest::newRow("0.07") << 0.07 << 0.01;
    QTest::newRow("1e7") << 1e7 << 1e6;
}

void TestGridMath::niceStep_examples()
{
    QFETCH(double, span);
    QFETCH(double, expected);

    const double step = GridMath::niceStep(span);
    COMPARE_EXACT(step, expected);
    QVERIFY(isCanonicalStep(step));
}

void TestGridMath::niceStep_invalidInput()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    COMPARE_EXACT(GridMath::niceStep(0.0), 0.0);
    COMPARE_EXACT(GridMath::niceStep(-0.0), 0.0);
    COMPARE_EXACT(GridMath::niceStep(-100.0), 0.0);
    COMPARE_EXACT(GridMath::niceStep(nan), 0.0);
    COMPARE_EXACT(GridMath::niceStep(inf), 0.0);
    COMPARE_EXACT(GridMath::niceStep(-inf), 0.0);
    COMPARE_EXACT(GridMath::niceStep(1000.0, 0), 0.0);
    COMPARE_EXACT(GridMath::niceStep(1000.0, -5), 0.0);
}

void TestGridMath::niceStep_maxLines()
{
    COMPARE_EXACT(GridMath::niceStep(1000.0, 5), 200.0);
    COMPARE_EXACT(GridMath::niceStep(1000.0, 1), 1000.0);
    COMPARE_EXACT(GridMath::niceStep(1000.0, 20), 50.0);
}

void TestGridMath::niceStep_property()
{
    // Логарифмическая развёртка 1e-2 .. 1e8: 10 декад по 200 точек.
    const int samples = 2001;
    for (int i = 0; i < samples; ++i) {
        const double span = std::pow(10.0, -2.0 + 10.0 * i / (samples - 1));
        const double step = GridMath::niceStep(span, 10);
        const double ratio = span / step;
        QVERIFY2(ratio > 4.0 && ratio <= 10.0,
                 QByteArray("span " + QByteArray::number(span, 'g', 17) + " step "
                            + QByteArray::number(step, 'g', 17) + " ratio "
                            + QByteArray::number(ratio, 'g', 17))
                     .constData());
        QVERIFY2(isCanonicalStep(step),
                 QByteArray("non-canonical step " + QByteArray::number(step, 'g', 17))
                     .constData());
        // Наименьший: предыдущее значение ряда (×0.5 после 1 и 2, ×0.4 после 5) уже даёт
        // больше 10 интервалов.
        const double previous = previousCanonicalStep(step);
        QVERIFY2(span / previous > 10.0,
                 QByteArray("step " + QByteArray::number(step, 'g', 17)
                            + " is not the smallest for span "
                            + QByteArray::number(span, 'g', 17))
                     .constData());
    }
}

void TestGridMath::gridLines_utmExample()
{
    COMPARE_LINES(GridMath::gridLines(5123401.3, 5124399.0, 200),
                  (std::vector<double>{5123600, 5123800, 5124000, 5124200}));
}

void TestGridMath::gridLines_boundaries()
{
    // Кратные шагу границы включаются.
    COMPARE_LINES(GridMath::gridLines(5123600.0, 5124200.0, 200),
                  (std::vector<double>{5123600, 5123800, 5124000, 5124200}));
    COMPARE_LINES(GridMath::gridLines(-200.0, 200.0, 100),
                  (std::vector<double>{-200, -100, 0, 100, 200}));
    // min == max на линии — одна линия, между линиями — ни одной.
    COMPARE_LINES(GridMath::gridLines(400.0, 400.0, 200), (std::vector<double>{400}));
    COMPARE_LINES(GridMath::gridLines(401.0, 599.0, 200), (std::vector<double>{}));
    // Граница в пределах допуска 1e-9 шага считается кратной.
    COMPARE_LINES(GridMath::gridLines(400.0 + 1e-8, 800.0 - 1e-8, 200),
                  (std::vector<double>{400, 600, 800}));
}

void TestGridMath::gridLines_fractionalStep()
{
    COMPARE_LINES(GridMath::gridLines(0.0, 0.5, 0.1),
                  (std::vector<double>{0.0, 0.1, 0.2, 0.3, 0.4, 0.5}));
    // Большие UTM-значения с мелким шагом: без хвостов вида 5123400.0000001.
    COMPARE_LINES(GridMath::gridLines(5123400.0, 5123400.05, 0.01),
                  (std::vector<double>{5123400.0, 5123400.01, 5123400.02, 5123400.03,
                                       5123400.04, 5123400.05}));
    COMPARE_LINES(GridMath::gridLines(-0.25, 0.25, 0.05),
                  (std::vector<double>{-0.25, -0.2, -0.15, -0.1, -0.05, 0.0, 0.05, 0.1,
                                       0.15, 0.2, 0.25}));
}

void TestGridMath::gridLines_invalidInput()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    QVERIFY(GridMath::gridLines(0.0, 1000.0, 0.0).empty());
    QVERIFY(GridMath::gridLines(0.0, 1000.0, -100.0).empty());
    QVERIFY(GridMath::gridLines(1000.0, 0.0, 100.0).empty());
    QVERIFY(GridMath::gridLines(nan, 1000.0, 100.0).empty());
    QVERIFY(GridMath::gridLines(0.0, nan, 100.0).empty());
    QVERIFY(GridMath::gridLines(0.0, 1000.0, nan).empty());
    QVERIFY(GridMath::gridLines(-inf, 1000.0, 100.0).empty());
    QVERIFY(GridMath::gridLines(0.0, inf, 100.0).empty());
    QVERIFY(GridMath::gridLines(0.0, 1000.0, inf).empty());
}

void TestGridMath::gridLines_maxCount()
{
    // 0..1000 с шагом 1 — 1001 линия: больше лимита по умолчанию.
    QVERIFY(GridMath::gridLines(0.0, 1000.0, 1.0).empty());
    QCOMPARE(GridMath::gridLines(0.0, 999.0, 1.0).size(), size_t(1000));
    QCOMPARE(GridMath::gridLines(0.0, 1000.0, 100.0, 11).size(), size_t(11));
    QVERIFY(GridMath::gridLines(0.0, 1000.0, 100.0, 10).empty());
    QVERIFY(GridMath::gridLines(0.0, 1e300, 1e-300).empty());
}

void TestGridMath::labelDecimals_examples_data()
{
    QTest::addColumn<double>("step");
    QTest::addColumn<int>("expected");

    QTest::newRow("100") << 100.0 << 0;
    QTest::newRow("1") << 1.0 << 0;
    QTest::newRow("0.5") << 0.5 << 1;
    QTest::newRow("0.1") << 0.1 << 1;
    QTest::newRow("0.05") << 0.05 << 2;
    QTest::newRow("0.01") << 0.01 << 2;
    QTest::newRow("0.001") << 0.001 << 3;
    QTest::newRow("1e-6") << 1e-6 << 6;
}

void TestGridMath::labelDecimals_examples()
{
    QFETCH(double, step);
    QFETCH(int, expected);

    QCOMPARE(GridMath::labelDecimals(step), expected);
}

QTEST_APPLESS_MAIN(TestGridMath)

#include "tst_gridmath.moc"
