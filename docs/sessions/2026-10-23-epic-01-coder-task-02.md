# 2026-10-23 · epic 01 · coder · task 02

Таск: ADR [0001](../adr/0001-coordinate-grid.md), Task 02 «Адаптивная сетка и подписи в UberPainter».
Issue [#11](https://github.com/vladmesh/Navigation/issues/11), PR в `release/epic-01` из `epic-01/task-02-grid-uberpainter`.

## Что сделано

- `Drawing/uberpainter.h/.cpp`: удалены `drawCoordinatesNet` и `updateCoordinates` (с объявлениями),
  вместе с ними ушли `maxX -= 50`, подписи по долям `/8`, `10.0 / transform().m11()`.
- Новый `drawGrid()`:
  - видимая область `mapToScene(viewport()->rect()).boundingRect()`, шаг
    `GridMath::niceStep(min(w, h), 10)`, линии `GridMath::gridLines` по области, расширенной на шаг;
  - линии `QGraphicsLineItem`, косметическое перо ширины 0, светло-серое; `gridGroup->setZValue(-10)`;
  - подписи `QGraphicsSimpleTextItem`, 8 pt, `ItemIgnoresTransformations`, текст
    `QString::number(v, 'f', labelDecimals(step))`; X у нижнего края, Y у левого; смещение 3 px
    задано собственным transform элемента (для такого элемента он в пикселях);
  - все элементы — дети одного `gridGroup`; перерисовка удаляет только его.
- Вызовы: `updateView()` (после `clear()`, `gridGroup` обнуляется до вызова), `mouseMoveEvent`
  при пане левой кнопкой (после `QGraphicsView::mouseMoveEvent`, когда вид уже сдвинут),
  `wheelEvent`, новый `resizeEvent`. В обработчиках `QMutexLocker(&sceneMutex)` вокруг вызова.
- `resizeEvent` берёт мьютекс через `try_lock`: `setSceneRect` внутри `updateView()` может
  синхронно показать/скрыть полосы прокрутки и вызвать ресайз вьюпорта, пока мьютекс захвачен.
  В этом случае сетку и так рисует `updateView()` следом, повторного захвата нет.
- `setSceneRect` в `updateView()` не менялся (ADR Decision §2).

## Как проверено

- Локально, Linux, Qt 6.8.3 gcc_64: `Ship.pro` собирается, предупреждений в `uberpainter.cpp` от GCC
  `-Wall -Wextra` нет (остальные предупреждения проекта старые). clang-format-diff по изменённым строкам чистый.
- Offscreen-стенд (вне репо: `UberPainter` + `ItemsStorage` + один `FixedItem` в UTM 512400/5123600,
  `QT_QPA_PLATFORM=offscreen`), проверки программные по элементам сцены:
  - без объектов: шаг 100, 5 линий по короткой стороне, покрытие всех краёв;
  - один объект; зум колесом от ~1 м (шаг 0.1, подписи `5123600.4`) до ~10 км (шаг 2000)
    за 67 щелчков, 13 смен шага, на каждом щелчке 4–11 линий по короткой стороне и покрытие краёв;
  - пан левой кнопкой (во время и после), ресайз 1200×400 и 400×900, линейка правой кнопкой
    (2 текстовые подписи есть во время протяжки);
  - каждая подпись стоит на своей линии и равна её координате, без экспоненты; у всех подписей
    `ItemIgnoresTransformations`; линии косметические и под объектами. Итог: `FAILURES: 0`.
  - скриншоты сохранены локально, в репо не коммитятся.
- PR-CI: run 1 на этом PR (см. STATUS.md).
- Не проверено: сборка на Windows и визуальная проверка на Windows-хосте (это у CI и тестера).

## Ревью

Раунды: 0 на момент открытия PR.
