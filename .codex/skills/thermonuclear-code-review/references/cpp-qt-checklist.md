# C++/Qt correctness checklist

Walk every item against the diff and the code it touches. An item is either "n/a",
"ok (why)", or a finding.

## Undefined behaviour

- Out-of-bounds indexing: `QString::at/[]`, `QByteArray[]`, `QList::at`, raw arrays,
  `mid/left/right` with computed lengths. Indexing "from the end" (`size() - 3`) on input
  that can be shorter than expected.
- Use of uninitialised members (POD members without default initialisers, especially in
  classes with several constructors).
- Dangling references/pointers: returning references to locals or temporaries,
  `QString::toUtf8().constData()` kept beyond the full expression, iterators/pointers kept
  across container modification, `QStringView`/`QLatin1StringView` outliving the source.
- Signed overflow, shift by >= width, division by zero (speed/heading/time deltas that can
  be zero), `std::abs(INT_MIN)`.
- Strict aliasing / `reinterpret_cast` of byte buffers to structs (SEG-Y, binary
  protocols): use `memcpy` or `QDataStream` with explicit byte order.
- Lambdas capturing `this` or locals by reference that run later (timers, queued
  connections, `QtConcurrent`).

## Ownership, raw pointers, RAII

- Every `new` has exactly one owner: Qt parent, `std::unique_ptr`, or a documented owner.
  A raw `new` with no parent and no delete is a leak; a `delete` on a parented `QObject`
  outside its own lifecycle is a double-delete risk.
- `QFile*`, `QTextStream*`, sockets, buffers allocated on the heap without a reason:
  prefer members by value or `std::unique_ptr`.
- Ownership transfer is explicit in the API (`std::unique_ptr` parameter, or a comment +
  `setParent`). Containers of raw owning pointers (`QList<Foo*>`) need `qDeleteAll` at a
  single, obvious place.
- Resources released on every path including early returns and exceptions: files closed,
  ports closed, mutexes unlocked via `QMutexLocker`/`std::lock_guard`.
- Rule of 0/3/5: a class that owns a resource either forbids copy (`Q_DISABLE_COPY` or
  `= delete`) or implements it correctly. `QObject` subclasses are never copied.
- Prefer `QPointer` for non-owning references to `QObject`s that may die first.

## Qt object model

- Parent/child: a widget or object created with a parent must not also be owned by a
  smart pointer. Objects reparented by layouts or `QGraphicsScene::addItem` (scene owns
  items) are not deleted manually afterwards.
- `deleteLater()` for objects that may be inside their own signal emission.
- `QGraphicsItem` lifetime vs the model object in `Objects/`: who deletes which, and in
  what order on scheme reload (`DataVault/ItemsLoader`).
- `Q_OBJECT` present for classes with signals/slots; new classes registered in `Ship.pro`
  so moc runs.

## Signals and slots

- New-style `connect(&Sender::signal, receiver, &Receiver::slot)` only; string-based
  `SIGNAL()/SLOT()` in new code is a finding.
- Context object given for lambda connections, so the connection dies with the receiver.
- Connection type correct across threads (`Qt::QueuedConnection` default is fine;
  `DirectConnection` across threads is a race; `BlockingQueuedConnection` in the same
  thread deadlocks).
- No duplicate connections when a dialog/connection is re-opened (`Qt::UniqueConnection`
  or disconnect on teardown).
- Arguments of queued signals are copyable and registered (`qRegisterMetaType`) if custom.
- Emitting signals from destructors or while holding a lock.

## Threads and races

- Which thread owns each `QObject` (`moveToThread`), and are all accesses from that thread?
  `QSerialPort` and `QTcpSocket` must be used only from their owner thread.
- Shared state (`ItemsStorage`, `CircularBuffer`, coordinator data) accessed from more than
  one thread without a mutex or a queued hand-off.
- No blocking calls (`waitForReadyRead`, `waitForConnected`, long file I/O, gradient descent
  in `Coordinator`) on the GUI thread.
- Thread shutdown: `quit()` + `wait()` before destroying objects; no detached threads.

## Exceptions and error handling

- Qt does not throw; standard library calls (`std::stoi`, `std::vector::at`, `new`) can.
  Exceptions must not escape a slot or an event handler (Qt aborts).
- Every `QString::toDouble/toInt` uses the `bool *ok` overload when input is external,
  and the failure branch does something sane.
- I/O results checked: `QFile::open`, `QIODevice::write` return values, socket `errorOccurred`,
  `QSerialPort::open` with a user-visible message.

## Integers and conversions

- `qsizetype`/`int` narrowing (Qt 6 sizes are `qsizetype`), `size_t` vs `int` comparisons,
  `double` → `int` truncation of coordinates, time stamps crossing midnight in NMEA.
- Degree/minute conversion and hemisphere sign for `ddmm.mmmm`; angles wrapped to
  [0, 360) / [-180, 180) consistently.
- Length-prefixed protocol (`Sender` writes a length before the JSON): width, byte order
  and signedness explicit.

## Parsing external data (NMEA, sockets, COM, files)

- Treat every byte from a GNSS receiver, buoy, socket, or log file as hostile.
- NMEA: starts with `$` or `!`, has `*` followed by exactly two hex digits, checksum over
  the right range, field count checked before indexing, empty fields handled, max sentence
  length (82 chars per spec) enforced or tolerated explicitly.
- Stream framing: data from `readyRead` may contain half a sentence or several sentences;
  a buffer accumulates and is split on `\r\n`; the buffer is bounded so a noisy port
  cannot grow memory without limit.
- Encoding: `QString::fromLatin1`/`fromUtf8` chosen deliberately.
- P1/90, PPK, Marimag, SEG-Y readers: fixed-width columns checked for line length,
  binary headers for file size before reading.
- Malformed input causes a logged rejection, never a crash or a silently wrong position.

## Windows / MSVC / MinGW

- Builds with both MSVC and MinGW if the ADR requires both; no GCC-only extensions,
  no `#include <bits/...>`, no VLA.
- Paths: `QDir::homePath()`/`QStandardPaths`, never `"~"` or hard-coded `C:/Users/...`;
  `QDir::toNativeSeparators` for display only.
- COM port names (`COM10` and above), `QSerialPortInfo` for enumeration.
- Windows headers: `NOMINMAX` before `<windows.h>` if it is included; no `min/max` macro
  clashes.
- Text files: line endings and `QIODevice::Text` used deliberately for logs.
- `windeployqt` step in `Ship.pro` still works when new Qt modules are added to `QT +=`.

## ADR and acceptance

- Each acceptance criterion maps to code and to a check (test, CI job, or a manual step
  the tester can repeat).
- Decisions recorded in the ADR are not silently overridden by the implementation.
