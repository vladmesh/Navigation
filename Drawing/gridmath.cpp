#include "gridmath.h"

#include <cmath>
#include <limits>

namespace {

// 10^n для n >= 0. До 10^22 степени десяти представимы в double точно, поэтому
// перемножение без накопления ошибки; дальше — std::pow (может вернуть inf).
double pow10Positive(int n)
{
    if (n > 22)
        return std::pow(10.0, n);
    double result = 1.0;
    for (int i = 0; i < n; ++i)
        result *= 10.0;
    return result;
}

// Ближайший double к mantissa × 10^exponent. Для отрицательной степени делим на точное
// 10^-n: одно деление даёт корректно округлённый результат, совпадающий с литералом.
double scaled(int mantissa, int exponent)
{
    if (exponent >= 0)
        return mantissa * pow10Positive(exponent);
    const double divisor = pow10Positive(-exponent);
    if (std::isfinite(divisor))
        return mantissa / divisor;
    return mantissa * std::pow(10.0, exponent); // субнормальные шаги
}

} // namespace

namespace GridMath {

double niceStep(double span, int maxLines)
{
    if (!std::isfinite(span) || span <= 0.0 || maxLines < 1)
        return 0.0;

    // log10 может ошибиться на единицу у точных степеней десяти, поэтому начинаем на
    // порядок ниже и перебираем кандидатов по возрастанию: первый подходящий — наименьший.
    const int startExponent =
        static_cast<int>(std::floor(std::log10(span / maxLines))) - 1;
    static const int mantissas[] = {1, 2, 5};

    for (int exponent = startExponent; exponent <= startExponent + 3; ++exponent) {
        for (int mantissa : mantissas) {
            const double step = scaled(mantissa, exponent);
            if (!(step > 0.0) || !std::isfinite(step))
                continue;
            if (span / step <= maxLines)
                return step;
        }
    }
    return 0.0; // недостижимо для конечного span, кроме переполнения шага
}

std::vector<double> gridLines(double min, double max, double step, int maxCount)
{
    std::vector<double> lines;
    if (!std::isfinite(min) || !std::isfinite(max) || !std::isfinite(step))
        return lines;
    if (step <= 0.0 || min > max)
        return lines;

    const double lowRatio = min / step;
    const double highRatio = max / step;
    if (!std::isfinite(lowRatio) || !std::isfinite(highRatio))
        return lines;

    // Допуск 1e-9 шага (в единицах отношения — 1e-9). На UTM-значениях с мелким шагом
    // (5e6 / 0.01 = 5e8) он меньше ulp отношения, поэтому не опускаем его ниже 4 ulp.
    const double eps = std::numeric_limits<double>::epsilon();
    const double lowTol = std::fmax(1e-9, 4.0 * eps * std::fabs(lowRatio));
    const double highTol = std::fmax(1e-9, 4.0 * eps * std::fabs(highRatio));

    const double first = std::ceil(lowRatio - lowTol);
    const double last = std::floor(highRatio + highTol);
    const double count = last - first + 1.0;
    if (count <= 0.0)
        return lines;
    if (count > maxCount)
        return {};

    const int decimals = labelDecimals(step);
    const double scale = decimals > 0 ? pow10Positive(decimals) : 1.0;
    const int n = static_cast<int>(count);
    lines.reserve(static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) {
        double value = (first + i) * step;
        if (decimals > 0 && std::isfinite(scale))
            value = std::round(value * scale) / scale;
        lines.push_back(value);
    }
    return lines;
}

int labelDecimals(double step)
{
    if (!std::isfinite(step) || step <= 0.0 || step >= 1.0)
        return 0;
    // +1e-9 гасит ошибку log10 у точных степеней (log10(0.001) = -2.9999999999999996).
    return -static_cast<int>(std::floor(std::log10(step) + 1e-9));
}

} // namespace GridMath
