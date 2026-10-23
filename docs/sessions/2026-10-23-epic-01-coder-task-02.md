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
- PR-CI run 1 (`0510c24`): MSVC и Linux ASan упали на линковке `tst_critical`: `Test/unit/critical/critical.pro`
  берёт исходники из `Test/app_sources.pri`, где был `uberpainter.cpp`, но не было `gridmath.cpp`
  (`LNK2019 GridMath::niceStep`). MinGW smoke и lint прошли.
- Исправление: `Drawing/gridmath.cpp/.h` добавлены в `Test/app_sources.pri`. Файл вне буквального scope
  Task 02 («только UberPainter»), но его шапка требует повторять в нём `Ship.pro`; `.pri` пришёл из `main`
  уже после Task 01, поэтому Task 01 его не обновил. Логики не меняет. Локально `tst_critical` 9/9,
  стенд FAILURES 0. PR-CI run 2.
- Не проверено: сборка на Windows и визуальная проверка на Windows-хосте (это у CI и тестера).

## Ревью

Раунд 1: GREEN-WITH-NITS (`docs/sessions/2026-10-23-epic-01-reviewer-task-02-round-1.md`), 4 minor.

Исправлено coder-ом (findings 1–2):
- [1] Подписи у краёв: подпись, которая не помещается во вьюпорт целиком, и Y-подпись в полосе X-подписей
  у нижнего края не создаются (проверка в пикселях через `mapFromScene`), линия остаётся.
- [2] Сетка перестраивается в `scrollContentsBy` (любой сдвиг: пан мышью, полосы прокрутки, клавиши) с
  `try_lock`, как в `resizeEvent`; отдельный вызов в `mouseMoveEvent` убран.
- Найдено стендом под ASan после [2]: heap-use-after-free в деструкторе. `delete graphicsScene` удалял
  `gridGroup` через `clear()`, затем сам звал `setScene(nullptr)`, пересчёт полос прокрутки вызывал
  `scrollContentsBy` → `drawGrid` → `delete` висячего указателя. Исправлено: деструктор сначала
  `setScene(nullptr)` и обнуляет `gridGroup`; `drawGrid` выходит, если сцена отвязана.
- Стенд дополнен: прокрутка полосами, проверка, что каждая подпись целиком внутри вьюпорта. Под
  ASan+UBSan `FAILURES: 0`, диагностик нет.

Оставлены для architect-revision (вне scope Task 02 по ADR):
- [3] `setSceneRect` без объектов получает `±DBL_MAX`/`-inf` (было до PR; ADR запрещает трогать `setSceneRect`).
- [4] Формулировка критерия «5–10 линий»: по Decision §1 это `span/step ∈ (4, 10]`, т.е. 4–11 видимых линий.

Ниты разрешено чинить без нового раунда, но правка заметная (новый обработчик и фикс UAF), поэтому
запрошен раунд 2.
