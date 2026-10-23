# 2026-10-23 · epic 01 · documentator

Вход: артефакт `Ship-epic-01-rc1` (release-build run 40017826744), `docs/sessions/2026-10-23-epic-01-builder.md`,
ADR 0001, `release/epic-01` @ `1fe3149`.

## Сверка

| Что | Было | Факт (код/сборка) | Правка |
|---|---|---|---|
| README: карта и сетка | не описаны | `UberPainter::drawGrid`, `GridMath`, ADR Decision §1–2 | новый раздел «Карта» |
| README: юнит-тесты | `tst_parsers`, `tst_critical` | плюс `Test/unit/gridmath/tst_gridmath.pro`, шаг в PR-CI | команда запуска, список тестов MSVC-job |
| README: `release-build.yml` | «вручную, zip `Navigation-<version>-win64`» | dispatch + push `release/**` + теги `v*`; zip `Ship-<version>`; dumpbin; smoke-старт без Qt в PATH | строка переписана |
| Build-инструкции | qmake + nmake/mingw32-make, Qt 6 | builder: Qt 6.8.3 MSVC 2022, `qmake Ship.pro CONFIG+=release`, nmake, `windeployqt --release --no-compiler-runtime` | совпадает с README/контрактом, без правок |
| ADR Documentation impact | 2 пункта | оба сделаны | отмечено, где описано |

Порты/протоколы, NMEA, `Sender`, пути файлов эпик не меняет; эти разделы не трогал. `AGENTS.md` в репо нет.

## Расхождения код ↔ ADR

Не найдено. Отличия, внесённые ревью (`scrollContentsBy`, `try_lock`, порядок в деструкторе, критерий
«4–11 линий»), architect-revision уже записал в ADR. Известное ограничение без объектов записано в ADR как leftover;
в README не выносил: это внутреннее состояние сцены, а не поведение для пользователя.

## Изменения

- `README.md`: §«Карта», `tst_gridmath`, `release-build.yml`.
- `docs/adr/0001-coordinate-grid.md`: Documentation impact — отмечено, что и где описано.
- PR в `release/epic-01` (docs-only), вместе с отчётом builder и STATUS.

## Next

`tester`, hand-off: артефакт `Ship-epic-01-rc1` (run 40017826744), этот docs PR, критерии ADR 0001 Task 01–02.
