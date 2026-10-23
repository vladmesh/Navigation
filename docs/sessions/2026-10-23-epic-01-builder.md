# 2026-10-23 · epic 01 · builder

Вход: `release/epic-01` @ `1fe3149` (Task 01 и Task 02 влиты, PR #5 и #12).

## Попытки

| # | Как | Результат |
|---|---|---|
| 1 | `release-build` через `workflow_dispatch` на `release/epic-01`, `version=epic-01-rc1`, run [40017826744](https://github.com/vladmesh/Navigation/actions/runs/40017826744) | success: сборка, упаковка, smoke-старт |

`build_attempts` = 1 из 5. Тот же коммит параллельно собрал push-триггер (run 40017801577), в попытки не
засчитан: это не запуск builder.

## Артефакт

- Имя: `Ship-epic-01-rc1` (zip `Ship-epic-01-rc1.zip`), 32 531 165 байт (артефакт Actions), хранится до 2027-01-21.
- sha256 zip: `d896720e3d3d843c617e3904f30a2d7d4539eba7f48675668e3219b7428b2dda`.
- Toolchain: Qt 6.8.3 `win64_msvc2022_64`, qmake + nmake, `CONFIG+=release`, `windeployqt --release
  --no-compiler-runtime`, MSVC runtime app-local из `VCToolsRedistDir`.

## Smoke-проверка

- В zip есть `Ship.exe`, `platforms/qwindows.dll`, `Qt6Core/Gui/Widgets/Network/SerialPort/Positioning.dll`,
  `msvcp140*.dll`, `vcruntime140*.dll`.
- Импорты `Ship.exe` (objdump): Qt6Widgets, Qt6Gui, Qt6Network, Qt6SerialPort, Qt6Positioning, Qt6Core,
  MSVCP140, VCRUNTIME140(_1), системные. Все Qt/MSVC-зависимости лежат в пакете (это же проверяет шаг
  dumpbin в workflow). `Qt6Location.dll` в пакет не попал: `Ship.exe` его не импортирует (линкер MSVC
  выкинул неиспользуемую зависимость), это не дефект.
- Workflow: распакован в чистую папку, Qt убран из PATH, `Ship.exe` жив через 15 с (шаг «Smoke start from
  clean folder» success).
- Не проверено: запуск на отдельном Windows-хосте, не на раннере GitHub. Это задача tester.

## Next

`documentator`, hand-off: артефакт run 40017826744 (`Ship-epic-01-rc1`), этот отчёт.
