# 2026-10-23 · epic 01 · reviewer · task 01 · round 1

Reviewer: independent `claude -p` process, read-only tools, run from a clean clone of `epic-01/task-01-gridmath` at `18edd2f`. Posted on PR #5 as a review comment.

## Thermonuclear review - PR #5, round 1/3
Verdict: RED

### Acceptance criteria
- [x] `gridmath.h/.cpp` include no Qt headers, and the API matches Decision §1 exactly. Evidence: `Drawing/gridmath.h:4` includes only `<vector>`, and `Drawing/gridmath.cpp:3-4` includes only `<cmath>` and `<limits>`. The signatures and default arguments at `Drawing/gridmath.h:23,35,42` are identical to the ADR.
- [x] `niceStep` examples (1000→100, 999→100, 1001→200, 50→5, 12345→2000, 2.5→0.5, 7→1, 0.07→0.01, 1e7→1e6), each canonical with no float tails. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:99-123` uses an exact `==` plus `isCanonicalStep`. CI: tst_gridmath 27/27 on MSVC and on GCC ASan+UBSan.
- [x] `niceStep` returns 0 for 0, negative, NaN, ±inf and `maxLines<1`. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:125-138`, `Drawing/gridmath.cpp:38-39`.
- [x] Property: `span/step` is in (4, 10] for at least 1000 log-spaced spans in 1e-2..1e8. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:147-169` checks 2001 samples and also checks that the step is the smallest one that fits.
- [x] `gridLines(5123401.3, 5124399.0, 200)` returns exactly {5123600, 5123800, 5124000, 5124200}. Boundaries that are multiples of the step are included. Invalid input and exceeding `maxCount` give an empty result. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:171-188, 200-224`.
- [x] `labelDecimals`: 100→0, 1→0, 0.5→1, 0.01→2. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:226-247`.
- [x] `Ship.pro` builds with the new files, and `tst_gridmath` builds and passes under `QT_QPA_PLATFORM=offscreen`. Evidence: `Ship.pro:34,72`. PR-CI run 40006604003 passed the MSVC /W4 build with tst_gridmath 27/0, the MinGW 6.10.2 build plus smoke test, the Linux full-app smoke compile, and the Linux ASan+UBSan run with 27 passed.

### Findings
1. [blocker] `Drawing/gridmath.cpp:43`: Undefined behaviour for some finite `span > 0`, which the contract says is valid input. When `span / maxLines` underflows to 0, `std::log10(0)` returns `-inf`. `static_cast<int>(-inf)` is then UB under [conv.fpint], and on x86 it yields INT_MIN, so the following `- 1` is signed overflow. That is a second UB.
   - **Failure scenario:** `niceStep(std::numeric_limits<double>::denorm_min(), 10)`, or `niceStep(1e-320, 1 << 30)`. The CI UBSan job does not catch this: GCC's `-fsanitize=undefined` does not include `float-cast-overflow`, and no test exercises subnormal spans.
   - **Reachability:** Task 02 cannot hit this from a real viewport. But the function is a public API whose header (`gridmath.h:17-21`) promises a defined result for every finite positive span.
   - **Fix:** Compute `const double target = span / maxLines;` and `if (!(target >= std::numeric_limits<double>::min())) return 0.0;` before the `log10`. Alternatively, clamp `std::floor(std::log10(target))` to [-330, 310] before the cast. Then document the narrowed domain in `gridmath.h:21` ("or span / maxLines is subnormal"). Add a test row for `denorm_min()`.

2. [major] `Drawing/gridmath.cpp:85,91-92`: `gridLines` returns values that are not multiples of `step` whenever the step needs more decimals than `labelDecimals(step)`.
   - **Why:** The values are rounded to `labelDecimals(step)` digits. That count is correct only for single-significant-digit steps like the ones `niceStep` produces. The ADR (Decision §1) and `gridmath.h:26-33` accept any finite `step > 0` and promise "values that are multiples of step".
   - **Failure scenario:** `gridLines(0, 1, 0.25)`: `labelDecimals(0.25)` is 1, so `round(2.5)/10` and `round(7.5)/10` give {0, 0.3, 0.5, 0.8, 1} instead of {0, 0.25, 0.5, 0.75, 1}. Likewise `gridLines(0, 0.1, 0.025)` gives 0.03 and 0.08 for 0.025 and 0.075. These are silently wrong line positions with no error.
   - **Fix:** Derive the rounding precision from the step itself, not from the label format. Find the smallest `d` in 0..15 for which `step * 10^d` is within a few ulp of an integer, then round to `d` decimals, and skip rounding if no such `d` exists. Alternatively, make it an explicit precondition that `step` must come from `niceStep` (document it in `gridmath.h` and in the ADR via architect-revision), and return empty for non-canonical steps. Either way, add a regression test with step 0.25 and step 0.025.

3. [nit] `Test/unit/gridmath/tst_gridmath.cpp:223`: `gridLines(0.0, 1e300, 1e-300)` is presented as a `maxCount` case. It actually returns empty through the `!std::isfinite(highRatio)` branch (`gridmath.cpp:68-69`), so the case does not test what it claims. **Fix:** Move it to `gridLines_invalidInput` with a comment saying the ratio overflows. For `maxCount`, use a finite large ratio, e.g. `gridLines(0.0, 1e12, 1.0)`.

### Not reviewed / could not verify
- I could not compile or run my own probes in this session because the sandbox denied local execution. Findings 1 and 2 come from reading the code. The arithmetic behind them is exact (0.25 × 10 = 2.5 rounds half away from zero to 3; `denorm_min / 10` underflows to 0) and does not depend on the platform.
- `niceStep` exactness for spans outside about 1e-21..1e23 (the `std::pow` fallback in `pow10Positive`/`scaled`) is untested. MSVC's `pow` is not guaranteed to be correctly rounded there. This is far outside the UTM range, so it is not raised as a finding.
- I did not open the full PR-CI logs. I relied on the CI summary provided: run 40006604003, all jobs green, tst_gridmath 27/0 on MSVC /W4 and on Linux ASan+UBSan.
- `docs/epics/EN/STATUS.md` and the coder report were checked only for consistency with the task. The changes merged in from main via release/epic-01 are out of scope.
