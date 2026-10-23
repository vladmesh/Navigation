# Navigation (Ship)

Десктопное Qt6/C++17 приложение для навигации буксируемых объектов (Windows, qmake: `Ship.pro`).

## CI

GitHub Actions, все workflow в `.github/workflows/`:

- **`pr-ci.yml`** — на каждый push и pull request:
  - *Windows / MSVC* — сборка `Ship.pro` (Qt 6.8, MSVC 2022, nmake, `/W4` без `/WX`) и автономных юнит-тестов `Test/unit` (QtTest), прогон тестов.
  - *Windows / MinGW smoke e2e* — сборка `Ship.pro` тулчейном владельца (Qt 6.10.2 `win64_mingw`, MinGW 13.1.0), рядом кладётся только набор из `deploy/win64-mingw/`, затем `Ship.exe --smoke` под `offscreen` и `windows`; проверяется, что каждый файл набора нужен. См. `deploy/win64-mingw/README.md`.
  - *Linux / ASan+UBSan* — юнит-тесты под `-fsanitize=address,undefined` (LeakSanitizer включён), плюс smoke-сборка всего приложения под Linux.
  - *Lint* — `clang-format` только по изменённым строкам (`.clang-format` в корне, старый код не переформатирован), `cppcheck` (падает только на severity `error`, предупреждения выводятся в лог), `clang-tidy` (bugprone/clang-analyzer) по парсерам.
- **`release-build.yml`** — вручную (workflow_dispatch): release-сборка + `windeployqt`, zip-артефакт `Navigation-<version>-win64`.
- **`nightly-drmemory.yml`** — ночью и вручную: юнит-тесты под Dr. Memory на Windows, не блокирующая заготовка.

Юнит-тесты локально:

```sh
cd Test/unit && qmake unit.pro && make && QT_QPA_PLATFORM=offscreen ./tst_parsers
```

Юнит-тесты на исправления C1-C3 (`Test/unit/critical`, собирают исходники приложения через `Test/app_sources.pri`):

```sh
cd Test/unit/critical && qmake critical.pro && make && QT_QPA_PLATFORM=offscreen ./tst_critical
```

Старые тестовые сценарии (`test_postprocessing`, `testppkparser`, `testitemsloading`) лежат в `Test/legacy` и собираются отдельным бинарём `legacy.pro`, в `Ship.pro` не входят. Они работают на полевых данных, которых нет в репозитории. Путь к данным задаётся переменной `NAV_TEST_DATA_DIR` (по умолчанию `./testdata`). В CI они пропускаются.

Незаконченные фичи вынесены в `archive/` (см. `archive/README.md`), в сборку не входят.
