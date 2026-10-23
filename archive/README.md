# archive/

Незаконченные фичи и наработки, вынесенные из сборки. Ничего отсюда не входит
в `Ship.pro` и не компилируется. Структура каталогов повторяет исходную, файлы
перенесены через `git mv`, история доступна через `git log --follow`.

Источник: коммит `4ac56dd` (Initial import), код в `main` `aee8ae8` не менялся.
Номера (б1, а1 ...) соответствуют аудиту мёртвого кода.

## Целые файлы

| Путь | Что это | Готовность |
|---|---|---|
| `Service/logger.*` (б1) | Асинхронный файловый логгер: синглтон, очередь, свой поток, файл `HH_MM_DD_MM.log`, уровни Info/Warning/Error. Нигде не подключался. | Высокая, не хватает подключения. Перед использованием отдельно проверить `moveToThread(this)` в конструкторе и `delete this` в `suicide()`. |
| `Objects/marimag.*`, `Parsers/marimagparser.*`, `Test/marimagparsertest.*` (б2) | Буксируемый двухрыбный магнитометр MariMag: позиция по собственному логу (lat/lon, layback, baseline). Объект нигде не создавался. | ~30%. Рабочий только `getFullData(QString)`; `parseMariMagFile` закомментирован, остальное заглушки. Тест пустой. |
| `Connections/connection_udp.*` (б5) | Приём GNSS по UDP. Радиокнопки `TCP_RB/UDP_RB` в `connection_creator.ui` остались, их прячет `hideUDPPossibility()`. | ~40%. Приём и фильтрация есть, `reconnect()` пустой. Наследуется от старого `Connection : QWidget`, для сборки нужно портировать на `Connections/connectionlogic.*`. |
| `Connections/connection.*` (а1) | Старый базовый класс `Connection : QWidget`, заменён `connectionlogic.*` + `connectionui.*`. Сам по себе мусор, лежит здесь только потому, что от него наследуется `connection_udp`. | Не нужен. |
| `Connections/depthsensors.*` (б6) | Интерполяция глубины каналов косы между датчиками глубины и буями. | Низкая. Во внутреннем цикле `i++` вместо `j++` (бесконечный цикл), для хвостового буя берётся `leadBuoyDepth`, слот в классе без `QObject`, данные заполняются константой 0.2. |
| `Drawing/sidegraphicview.*` (б7) | Вид сбоку (профиль X/Z): судно и буксируемые объекты по глубине. Вызовы в `mainwindow.cpp` были закомментированы. | Отрисовка готова, интеграция удалена. Нужен ли вид сбоку, решает автор. |

## Фрагменты из живых файлов (`snippets/`)

Вырезаны из файлов, которые остаются в сборке. В шапке каждого файла:
откуда взят, объявления из заголовка, как вернуть.
Расширение `.cpp.txt`, чтобы фрагменты не попадали под clang-format и
cppcheck в CI: это не компилируемые единицы.

| Файл | Откуда | Что это | Готовность |
|---|---|---|---|
| `streamer-catenary.cpp.txt` (б3) | `Objects/streamer.cpp` | `calcChansByChainAlgorithm`, `findA` (Ньютон), `calculateAzimuth`: раскладка каналов по цепной линии между головой и хвостовым буем. Был выключен `#define CHAINALGO` перед `#ifdef`. | Средняя. Выражение без эффекта `realAzimuthOfTowingRadians + M_PI/2;`. Данные для проверки есть в ручном сценарии `testPostprocessing::testChainAlgo` (`Test/legacy`), но из-за `#define` он шёл по прямой. |
| `streamer-checkStreamerCoordinates.cpp.txt` (б12) | `Objects/streamer.cpp` | Проверка координат каналов. | Заглушка, возвращает 666. |
| `coordinator-source-multilateration.cpp.txt` (б4) | `Service/coordinator.cpp` | `updateSourcePosition` + `calculateError`: позиция источника градиентным спуском по расстояниям до приёмников. | Только алгоритм. Источник расстояний не определён. |
| `streamerdialog-csv.cpp.txt` (б8) | `Fabric/streamerdialog.cpp` | Импорт и экспорт раскладки косы в CSV. | Высокая, но кнопки не создавались; `saveToCSV` пишет захардкоженные глубины, одна строка с `,` вместо `;`, `readCSV` не читает `Channel spacing`. |
| `p190-multisource-header.cpp.txt` (б9) | `Parsers/p190_creator.cpp` | Строки P1/90 по каждому источнику, заполнение H0103, `setLineName`. | ~10%. Фильтр по `sourceID` был заменён на `if (true)`, циклы пустые. |
| `buoydialog-streamer-binding.cpp.txt` (б10) | `Fabric/buoydialog.cpp` | Выбор косы для хвостового буя в диалоге буя. | Закомментировано автором, связь теперь через `Streamer::setEndBuoy`. Скорее отказ от фичи. |
| `sendersettingsmenu-sending-checkbox.cpp.txt` (б11) | `MenuWindows/sendersettingsmenu.cpp` | Чекбокс «Sending» в настройках отправителя. | Закомментировано автором, вместо него кнопки Start/Stop. |
