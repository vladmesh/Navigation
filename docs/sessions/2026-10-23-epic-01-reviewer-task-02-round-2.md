# 2026-10-23 · epic 01 · reviewer · task 02 · round 2

Reviewer: independent `claude -p` process, read-only tools, clean clone at `363ce54`. Posted on PR #12: https://github.com/vladmesh/Navigation/pull/12#pullrequestreview-5633870038

## Thermonuclear review - PR #12, round 2/3
Verdict: GREEN-WITH-NITS

### Acceptance criteria
- [x] No `drawCoordinatesNet`, no `updateCoordinates`, no `maxX -= 50`, no `/8` labels. Evidence: a grep of Drawing/uberpainter.cpp and Drawing/uberpainter.h finds none of them. The only `mirrorTransform` left is in the ruler branch at Drawing/uberpainter.cpp:166-182, which is not grid code.
- [x] The step comes only from `GridMath::niceStep(min(w, h), 10)` and the lines only from `GridMath::gridLines`. Evidence: Drawing/uberpainter.cpp:44 and :54-55. UberPainter does no step arithmetic of its own. The margin at :53 is the one-step margin the ADR asks for.
- [x] The grid covers all four edges, with no objects and with one object. Evidence: the visible area plus one step (:42-43, :53); the object bbox is not used. The no-object state still runs on the broken scene rect from round 1 (finding 3 below, unchanged and outside this task).
- [x] 4 to 11 lines along the short side, and the step changes at once on the wheel. Evidence: :228-229 redraws at the end of `wheelEvent`. `scrollContentsBy` now also redraws during `scale()`/`centerOn`. The ADR wording problem is finding 4 below, unchanged.
- [x] Each label sits at its own line and equals its coordinate: X along the bottom edge, Y along the left edge, `'f'` format with `labelDecimals`. Evidence: :71-101. Checked under `scale(1,-1)`:
  - An X label's anchor is `(x, scene y of pixel row H-1)`. The device-space offset `(3, -h-3)` puts its top-left at `(px+3, H-4-h)`, so the label sits 3 px right of its line and 3 px above the bottom edge.
  - A Y label's anchor is `(scene x of pixel column 0, y)`. The offset `(3, -h)` puts it directly above its line, 3 px in from the left edge.
  - The new pixel checks at :87-93 are correct. They drop any label that would be cut by the viewport. They also drop a Y label whose bottom is below `H - h - 6`, which is 2 px above the top of the X row (`H - 4 - h`). So a Y label can no longer overlap the X row. Round 1 finding 1 is fixed.
- [x] Labels are not mirrored and keep the same size at any zoom; the grid code uses neither `mirrorTransform` nor `transform().m11()`. Evidence: :78 sets `ItemIgnoresTransformations` and :68-69 uses a fixed 8 pt font.
- [x] Mouse pan and window resize rebuild the grid at once, and the ruler labels survive a pan. Evidence: `scrollContentsBy` (:232-241) now covers mouse pan, scroll bar drags, the keyboard and `centerOn`. The pan-only block in `mouseMoveEvent` is gone (:190). Round 1 finding 2 is fixed.
  - Ruler items are top-level scene items created by `addText`/`addLine` (:172-173, :186). `delete gridGroup` (:39) only reaches the group's children.
  - A right-button drag does not scroll the view: `ScrollHandDrag` only starts on the left button. So nothing calls `drawGrid` during a ruler drag, and `run()` skips `updateView` while `isNotDrawingLine` is false.
- [x] A redraw deletes only grid items, nothing is deleted twice after `clear()`, and `sceneMutex` is never locked twice.
  - Ownership: `gridGroup` is created without a parent and handed to the scene by `addItem` (:48-50). Every line and label has `gridGroup` as its parent (:61, :63, :75). Labels dropped at :93 are deleted while still children, which is safe. `updateView` sets the pointer to null right after `clear()` (:258-259), and nothing runs in between. No other code calls `clear()` or `setScene`.
  - Destructor (the use-after-free fix): `setScene(nullptr)` (:22) runs while the scene and `gridGroup` are still alive. The scroll that `recalculateContentSize` triggers reaches `drawGrid`, which returns at :36 because `scene()` is already null. Then `gridGroup` is set to null (:23) and `delete graphicsScene` frees the items exactly once. When `~QGraphicsScene` runs, no views are attached to it any more. The base-class destructors that run after `~UberPainter` dispatch to the base `scrollContentsBy`. The fix is correct.
  - Constructor: `setScene` and `scale(1,-1)` (:11-13) can call the override while the constructor body runs. By then `sceneMutex`, `graphicsScene` and `gridGroup` are initialised, because they are declared before anything the body uses. `drawGrid` does not read the uninitialised `minX`..`maxY`. No undefined behaviour.
  - Locking: everything runs on the GUI thread. Only `updateView` (:257) and `wheelEvent` (:228) lock unconditionally, and neither runs an event loop or calls into the other.
    - Inside `updateView`, `setSceneRect` (:276) can scroll synchronously. That scroll reaches `scrollContentsBy`, whose `try_lock` fails and skips the redraw. Skipping is safe because `drawGrid` runs at :278, after the scroll.
    - Scroll bar show/hide reaches Qt's `_q_showOrHideScrollBars` through a queued connection. So the viewport resize, and with it `resizeEvent`, arrives after the lock is released.
    - `centerView()`/`centerOn` runs before the lock (:256).
    - `drawGrid` adds no item that can change the explicit scene rect synchronously.
    - Qt documents that `QMutex::tryLock` on a non-recursive mutex returns false, without deadlocking, when the owning thread calls it again.
- [x] Grid lines lie under tracks and objects, and the pen is cosmetic. Evidence: :49 sets the group's zValue to -10, and :57-59 makes the pen cosmetic with width 0.
- [x] Ship.pro builds on Windows with no new warnings in uberpainter.cpp. Evidence: PR-CI run 3 (363ce54), all 4 jobs green. MSVC /W4 shows no warnings for uberpainter.cpp, and the MinGW build plus smoke e2e passed. Linux ASan, UBSan and LSan are clean, including tst_critical, which links UberPainter.

### Findings
1. [minor] Drawing/uberpainter.cpp:232-241: `scrollContentsBy` rebuilds the whole grid on every scroll bar change, so one user action now causes several full rebuilds.
   - Every rebuild deletes and recreates about 30 to 70 items, and each label runs font layout.
   - Failure scenario: a left-button pan.
     - `mouseMoveEvent` sets the horizontal and then the vertical scroll bar (:152-153), which is 2 rebuilds.
     - The base `QGraphicsView::mouseMoveEvent` with `ScrollHandDrag` sets both bars again (:190), which is 2 more.
     - That makes up to 4 rebuilds per mouse move. Before this round it was 1.
   - One wheel notch causes about 2 rebuilds from `scale()`/`centerOn`, 1 from the base wheel scroll and 1 explicit rebuild at :229.
   - Every timer tick, `centerView()` → `centerOn` (:113, :256) builds a grid on each scrolled axis, and `clear()` deletes it a few lines later (:258).
   - None of this is wrong. But the ADR names lag on pan as its main risk, and this multiplies the per-move cost by up to 4 on the path the tester will exercise.
   - Fix: coalesce the rebuild.
     - Add `bool gridPending = false;`.
     - In `scrollContentsBy`, after the base call: `if (!gridPending) { gridPending = true; QTimer::singleShot(0, this, [this] { gridPending = false; QMutexLocker l(&sceneMutex); drawGrid(); }); }`.
     - Use the same helper in `resizeEvent` and `wheelEvent`. This removes both `try_lock` sites, because the deferred call never runs under the lock in `updateView`.
     - The `this` context object drops the call if the view is destroyed first.
     - Qt posts paint requests at low priority, so the rebuild still lands before the next paint and there is no stale frame.
2. [minor] (round 1 finding 3, carried, unchanged, owned by architect-revision) Drawing/uberpainter.cpp:276: with no objects, `setSceneRect` gets `(DBL_MAX, DBL_MAX, -inf, -inf)`.
   - Once zoomed in (m11 > 1), `DBL_MAX * m11` overflows to `inf`. Qt's `mapRect` then computes `inf - inf` = NaN, and the integer rounding of scroll bar ranges inside Qt receives that NaN.
   - In that state the grid labels describe a meaningless area.
   - The new code adds no new path into this state: `scrollContentsBy` only receives the int deltas Qt computes.
   - ADR line 79 forbids touching `setSceneRect` in this task. Fix: architect-revision records a new task to keep the previous scene rect, or use a default rect around the view centre, when no position passed the filter.
3. [minor] (round 1 finding 4, carried, unchanged, owned by architect-revision) docs/adr/0001-coordinate-grid.md:188: "5 to 10 lines" contradicts Decision §1, where span/step lies in (4, 10], which means 4 to 11 visible lines. Fix: reword the criterion before the tester runs.
4. [nit] docs/adr/0001-coordinate-grid.md:100-104: the ADR still lists `mouseMoveEvent` as a call site of `drawGrid`, but the code now uses `scrollContentsBy`, as the round 1 review asked. Fix: architect-revision updates Decision §2 to name `scrollContentsBy` and to describe the `try_lock` rule for handlers that can run inside `updateView`. This is so the ADR does not silently disagree with the code.

### Not reviewed / could not verify
- I did not run the app, did not check it on Windows, and did not measure the cost of a pan. Finding 1 rests on counting the synchronous `scrollContentsBy` calls, not on a timing. The coder's offscreen harness is still not in the repo, so I could not rerun it.
- No Qt sources were available locally, so I checked these Qt internals from memory rather than in the source:
  - `QGraphicsView::setScene(nullptr)` sets `d->scene` before `recalculateContentSize`.
  - `QGraphicsView::scrollContentsBy` returns early while `transforming` is set.
  - `rangeChanged` is connected to `_q_showOrHideScrollBars` with a queued connection.
  - `UpdateRequest` is posted at low priority.
  The locking conclusion does not depend on the queued connection, because `try_lock` covers the synchronous case too. The destructor conclusion is backed by the coder's ASan run and CI LSan.
- Problems that predate this PR, already listed in round 1 and not raised again:
  - The ruler pointers dangle after `clear()` and leak items on every right press.
  - A pan moves the view twice per mouse move (manual scroll plus `ScrollHandDrag`).
  - `wheelEvent` also scrolls the view vertically through the base handler.
  - X labels can overlap when the short side of a very small window gives less than about 45 px between lines.
- The changes to Test/app_sources.pri were accepted in round 1 and have not changed since.
