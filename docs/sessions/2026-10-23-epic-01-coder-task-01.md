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
- `8e7bd4b` этот отчёт и STATUS.md
- `bdf5811`, `e23d364` merge `release/epic-01` (там теперь `main` с PR-CI, PR #10); конфликт в
  `Ship.pro` (закомментированные `lordofgraphic`/`sidegraphicview` удалены в main) решён:
  оставлен только `Drawing/gridmath.cpp`
- `6730f63` шаги `tst_gridmath` в PR-CI (Windows MSVC /W4 и Linux ASan+UBSan+LSan), как
  разрешает Task 01; clang-format новых строк (`.clang-format` из main)

## Ревью

Раунд 1: RED (`docs/sessions/2026-10-23-epic-01-reviewer-task-01-round-1.md`). Раунд 2: GREEN-WITH-NITS
(`docs/sessions/2026-10-23-epic-01-reviewer-task-01-round-2.md`). Оба нита раунда 2 исправлены без нового
раунда (см. ниже). Открытых нитов нет.

## Заметки для следующих тасков

- Когда `ci/minimal` появится в release-ветке, `tst_gridmath` нужно добавить в PR-CI
  (ADR это разрешает в Task 01, сейчас workflow нет).
- Два регистрозависимых include выше ломают сборку на Linux-раннере, если CI будет на Linux.

## Обновление 2026-10-23 (после синка с main)

- В release-ветке появился PR-CI. `tst_gridmath` добавлен в jobs `Windows / MSVC` и
  `Linux / GCC ASan+UBSan(+LSan)`, JUnit-отчёт в артефакте `unit-test-results-windows`.
- Локально после синка: `tst_gridmath` 27/27, `Ship.pro` собирается (Qt 6.8.3 gcc_64),
  clang-format-diff по изменённым строкам чистый. Шимы include больше не нужны (починено в main).
- PR-CI прогоны по таску: см. `ci_runs` в STATUS.md и checks в PR #5.

## Исправления по ревью, раунд 1

Отчёт ревьюера: `docs/sessions/2026-10-23-epic-01-reviewer-task-01-round-1.md` (RED).

- [blocker] UB в `niceStep` при субнормальном `span / maxLines` (`log10(0) = -inf`, каст к int):
  ранний возврат 0, если `span / maxLines < DBL_MIN`; контракт в `gridmath.h` дополнен.
  Тесты: `denorm_min()`, `1e-320` при `maxLines = 2^30`, `DBL_MIN`, `DBL_MIN * 10`.
- [major] `gridLines` округлял по `labelDecimals(step)`, для шага 0.25 давал 0.3/0.8: теперь
  округление до числа знаков самого шага (`stepDecimals`, допуск 8 ulp), за 2^53 не округляем.
  Тесты: шаги 0.25, 0.025 и 0.25 на UTM-значении.
- [nit] `gridLines(0, 1e300, 1e-300)` перенесён в `gridLines_invalidInput`; в `maxCount`
  вместо него `gridLines(0, 1e12, 1)`.
- Локально: `tst_gridmath` под ASan+UBSan с `-fsanitize=float-cast-overflow` и
  `-fno-sanitize-recover=all` — 27/27, без диагностик; clang-format-diff чистый.

## Ниты раунда 2 (исправлены без нового раунда, как разрешает coder)

- [minor] `stepDecimals` для не десятичного шага (1/3, 1/7) находил d=15 и округлял: теперь при допуске
  от 0.5 возвращает -1, значения не округляются. Тест: `gridLines(0, 1, 1.0/3)` ровно `i * step`.
- [nit] `gridmath.h`: свойство `(maxLines/2.5, maxLines]` сформулировано для ненулевого результата.
- PR-CI: run 1 (`18edd2f`) и run 2 (`d166ee1`) зелёные 4/4; run 3 на этом коммите.
