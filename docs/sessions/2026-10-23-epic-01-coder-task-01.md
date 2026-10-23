# 2026-10-23 · epic 01 · coder · task 01

Таск: ADR [0001](../adr/0001-coordinate-grid.md), Task 01 «Чистая функция расчёта шага сетки + unit-тест».
Issue [#4](https://github.com/vladmesh/Navigation/issues/4), PR [#5](https://github.com/vladmesh/Navigation/pull/5)
(`epic-01/task-01-gridmath` → `release/epic-01`).

## Что сделано

- `Drawing/gridmath.h/.cpp`: `GridMath::niceStep`, `gridLines`, `labelDecimals`, сигнатуры
  как в ADR Decision §1; заголовков Qt нет (`<vector>`, `<cmath>`, `<limits>`). Добавлены в `Ship.pro`.
- `Test/unit/gridmath/tst_gridmath.pro` + `tst_gridmath.cpp`: `QT = core testlib`,
  `QTEST_APPLESS_MAIN`, `gridmath.cpp` компилируется напрямую.
- `UberPainter` и `.github/` не тронуты.

Решения внутри таска:
- `niceStep` строит кандидата как `m / 10^k` (точная степень десяти, одно деление), поэтому
  шаг совпадает с литералом (`0.01`, а не `0.010000000000000002`). Перебор начинается на
  порядок ниже `floor(log10(span/maxLines))`, первый подходящий и есть наименьший.
- `gridLines` считает в единицах `value/step`; допуск `1e-9`, но не меньше 4 ulp отношения
  (для northing ~5e6 и шага 0.01 отношение ~5e8, там 1e-9 меньше ulp). Значения
  округляются до `labelDecimals(step)` знаков.

## Покрытие критериев приёмки тестом

| Критерий | Тест |
|---|---|
| примеры `niceStep` (1000→100 … 1e7→1e6), канонический вид | `niceStep_examples` (точное `==` и проверка на {1,2,5}×10ⁿ) |
| 0, отрицательные, NaN, ±inf, `maxLines<1` → 0 | `niceStep_invalidInput` |
| свойство `span/step ∈ (4,10]` | `niceStep_property`: 2001 точка, лог-развёртка 1e-2..1e8, плюс проверка, что шаг наименьший |
| `gridLines(5123401.3, 5124399.0, 200)` точно | `gridLines_utmExample` |
| границы, кратные шагу, включаются | `gridLines_boundaries` |
| некорректный ввод, превышение `maxCount` → пусто | `gridLines_invalidInput`, `gridLines_maxCount` |
| `labelDecimals` 100→0, 1→0, 0.5→1, 0.01→2 | `labelDecimals_examples` |
| без хвостов на дробных шагах и UTM | `gridLines_fractionalStep` |

## Как проверено

- PR-CI: no workflow yet (в `release/epic-01` нет `.github/`). `ci_runs` = 0. Зелёным не считается.
- Локально, Linux, Qt 6.8.3 gcc_64 (aqtinstall, без sudo):
  - `qmake Test/unit/gridmath/tst_gridmath.pro && make && QT_QPA_PLATFORM=offscreen ./tst_gridmath`
    → `Totals: 27 passed, 0 failed`; предупреждений компилятора нет (`-Wall -Wextra`).
  - Мутация: точное деление заменено на `pow(10, n)`, округление в `gridLines` убрано →
    падает `gridLines_fractionalStep`. Тесты на точность значений работают.
  - `qmake Ship.pro CONFIG+=release && make`: все объектники собираются, `gridmath.o` без
    предупреждений, `Ship` линкуется. Для Linux понадобились локальные обходы, к PR не
    относящиеся: шимы для include `<QProgressbar>` (`Service/postprocessor.h`) и
    `<QGraphicsEllipseItem >` с пробелом (`Drawing/hoverpaintingrpoint.h`) на
    регистрозависимой ФС, симлинки libGL/xkbcommon и `--allow-shlib-undefined` для EGL.
    На Windows так собираться и должно, но сборка под MinGW/MSVC не запускалась.

## Коммиты

- `49a3fbf` epic-01/task-01: add GridMath (niceStep, gridLines, labelDecimals) and tst_gridmath
- следующий коммит: этот отчёт и STATUS.md

## Ревью

Раунды ревью: 0. Ревьюер запускается следующим шагом пайплайна по PR #5. Открытых нитов нет.

## Заметки для следующих тасков

- Когда `ci/minimal` появится в release-ветке, `tst_gridmath` нужно добавить в PR-CI
  (ADR это разрешает в Task 01, сейчас workflow нет).
- Два регистрозависимых include выше ломают сборку на Linux-раннере, если CI будет на Linux.
