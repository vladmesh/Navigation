# Navigation (Ship)

Десктопное Qt6/C++17 приложение для навигации буксируемых объектов (Windows, qmake: `Ship.pro`).

## Карта

Зона отрисовки (`Drawing/UberPainter`) показывает треки объектов в метрах UTM: X — easting, Y — northing
(ось Y направлена вверх).

- Координатная сетка включена всегда, переключателя нет. Она покрывает всю видимую область, в том числе
  когда объектов нет.
- Шаг сетки адаптивный, из ряда 1–2–5×10ⁿ м, один для обеих осей (квадратные ячейки). По короткой стороне
  окна укладывается от 4 до 10 шагов. Шаг меняется сразу при зуме колесом, пане мышью, прокрутке и
  изменении размера окна.
- Подписи — координата линии в метрах без единиц: X у нижнего края, Y у левого. Знаков после запятой
  столько, сколько нужно для шага (`512400` при шаге 100 м, `5123600.5` при шаге 0.5 м). Размер подписей
  не зависит от зума. Подпись, которая не помещается у края целиком, не показывается.
- Правая кнопка мыши — линейка: расстояние и азимут от точки нажатия.
- Расчёт шага и линий — `Drawing/gridmath.h` (`GridMath::niceStep`, `gridLines`, `labelDecimals`), решение —
  `docs/adr/0001-coordinate-grid.md`.

## CI

GitHub Actions, все workflow в `.github/workflows/`:

- **`pr-ci.yml`** — на каждый push и pull request:
  - *Windows / MSVC* — сборка `Ship.pro` (Qt 6.8, MSVC 2022, nmake, `/W4` без `/WX`) и автономных юнит-тестов `Test/unit` (QtTest: `tst_parsers`, `tst_critical`, `tst_gridmath`), прогон тестов.
  - *Windows / MinGW smoke e2e* — сборка `Ship.pro` тулчейном владельца (Qt 6.10.2 `win64_mingw`, MinGW 13.1.0), рядом кладётся только набор из `deploy/win64-mingw/`, затем `Ship.exe --smoke` под `offscreen` и `windows`; проверяется, что каждый файл набора нужен. См. `deploy/win64-mingw/README.md`.
  - *Linux / ASan+UBSan* — юнит-тесты под `-fsanitize=address,undefined` (LeakSanitizer включён), плюс smoke-сборка всего приложения под Linux.
  - *Lint* — `clang-format` только по изменённым строкам (`.clang-format` в корне, старый код не переформатирован), `cppcheck` (падает только на severity `error`, предупреждения выводятся в лог), `clang-tidy` (bugprone/clang-analyzer) по парсерам.
- **`release-build.yml`** — вручную (workflow_dispatch, параметр `version`) и на push в `release/**` и теги `v*`: release-сборка MSVC (Qt 6.8.3) + `windeployqt` + MSVC runtime рядом с exe, проверка зависимостей через `dumpbin`, smoke-старт из чистой папки без Qt в PATH; zip-артефакт `Ship-<version>` (без `version` — короткий sha).
- **`nightly-drmemory.yml`** — ночью и вручную: юнит-тесты под Dr. Memory на Windows, не блокирующая заготовка.

Юнит-тесты локально:

```sh
cd Test/unit && qmake unit.pro && make && QT_QPA_PLATFORM=offscreen ./tst_parsers
```

Юнит-тест расчёта сетки (`Test/unit/gridmath`, только QtCore + QtTest, без GUI):

```sh
cd Test/unit/gridmath && qmake tst_gridmath.pro && make && QT_QPA_PLATFORM=offscreen ./tst_gridmath
```

Юнит-тесты на исправления C1-C3 (`Test/unit/critical`, собирают исходники приложения через `Test/app_sources.pri`):

```sh
cd Test/unit/critical && qmake critical.pro && make && QT_QPA_PLATFORM=offscreen ./tst_critical
```

Старые тестовые сценарии (`test_postprocessing`, `testppkparser`, `testitemsloading`) лежат в `Test/legacy` и собираются отдельным бинарём `legacy.pro`, в `Ship.pro` не входят. Они работают на полевых данных, которых нет в репозитории. Путь к данным задаётся переменной `NAV_TEST_DATA_DIR` (по умолчанию `./testdata`): каталог, в котором лежат `Desktop/FINAL/...` и `Documents/Ship_logs/...`, как в домашней папке автора. В CI они пропускаются.

Незаконченные фичи вынесены в `archive/` (см. `archive/README.md`), в сборку не входят.
