#ifndef GRIDMATH_H
#define GRIDMATH_H

#include <vector>

/**
 * @brief Арифметика координатной сетки карты (ADR 0001, Decision §1).
 *
 * Только стандартная библиотека, без Qt: модуль собирается и тестируется отдельно
 * от GUI (Test/unit/gridmath). Координаты — метры UTM, но функции от единиц не зависят.
 */
namespace GridMath {

/**
 * @brief Наименьший шаг из ряда {1, 2, 5} × 10^n, при котором span / step <= maxLines.
 *
 * Результат равен ближайшему double к m × 10^n (тот же, что у литерала "2e-3"),
 * без хвостов вида 199.99999. Если результат ненулевой, отношение span / step лежит
 * в (maxLines / 2.5, maxLines].
 *
 * @return 0.0, если span <= 0, span не конечен, maxLines < 1 или span / maxLines
 *         субнормально (меньше DBL_MIN) — сетку не рисовать.
 */
double niceStep(double span, int maxLines = 10);

/**
 * @brief Значения линий сетки, кратные step, в отрезке [min, max].
 *
 * Первая линия = ceil(min / step) * step. Границы, кратные step, включаются (допуск
 * 1e-9 * step, но не меньше нескольких ulp). Значения округлены до числа знаков,
 * которым записывается сам step, поэтому 0.1-шаг даёт 0.3, а не 0.30000000000000004,
 * а 0.25-шаг даёт 0.75.
 *
 * @return пустой вектор, если step <= 0, min > max, аргумент не конечен или линий
 *         больше maxCount.
 */
std::vector<double> gridLines(double min, double max, double step, int maxCount = 1000);

/**
 * @brief Число знаков после запятой для подписи линии с шагом step.
 *
 * 0 при step >= 1, иначе -floor(log10(step)). Для step <= 0 и не конечных значений — 0.
 */
int labelDecimals(double step);

} // namespace GridMath

#endif // GRIDMATH_H
