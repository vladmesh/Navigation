# 2026-10-23 · epic 01 · reviewer · task 01 · round 2

Reviewer: independent `claude -p` process, read-only tools, clean clone at `d166ee1`. Posted on PR #5: https://github.com/vladmesh/Navigation/pull/5#pullrequestreview-5633301209

## Thermonuclear review - PR #5, round 2/3
Verdict: GREEN-WITH-NITS

### Acceptance criteria
- [x] `gridmath.h/.cpp` include no Qt headers, and the API matches Decision §1 exactly. Evidence: `Drawing/gridmath.h:4` includes only `<vector>`, and `Drawing/gridmath.cpp:3-4` includes only `<cmath>` and `<limits>`. The signatures and default arguments at `Drawing/gridmath.h:24,37,44` are unchanged from the ADR.
- [x] `niceStep` examples (1000→100, 999→100, 1001→200, 50→5, 12345→2000, 2.5→0.5, 7→1, 0.07→0.01, 1e7→1e6) return canonical values with no float tails. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:99-123` checks them with exact `==` plus `isCanonicalStep`. PR-CI run 2: tst_gridmath 27/0 on MSVC /W4 and on GCC ASan+UBSan.
- [x] `niceStep` returns 0 for 0, negative values, NaN, ±inf and `maxLines<1`. It now also returns 0 when `span / maxLines` is subnormal or underflows to 0. Evidence: `Drawing/gridmath.cpp:57-64`, `Test/unit/gridmath/tst_gridmath.cpp:125-144`.
- [x] Property: for 2001 log-spaced spans in 1e-2..1e8, `span/step` is in (4, 10] and the step is the smallest that fits. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:153-175`.
- [x] `gridLines(5123401.3, 5124399.0, 200)` returns exactly {5123600, 5123800, 5124000, 5124200}. Boundaries that are multiples of the step are included. Invalid input and exceeding `maxCount` return an empty result. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:177-194, 212-239`.
- [x] `labelDecimals`: 100→0, 1→0, 0.5→1, 0.01→2. Evidence: `Test/unit/gridmath/tst_gridmath.cpp:241-262`, `Drawing/gridmath.cpp:130-136`.
- [x] `Ship.pro` builds with the new files, and `tst_gridmath` builds and passes under `QT_QPA_PLATFORM=offscreen`. Evidence: `Ship.pro:34,72`. PR-CI run 2 on d166ee1 passed everything: MSVC /W4 build with tst_gridmath 27/0 (JUnit), MinGW 6.10.2 build plus smoke test, Linux ASan+UBSan+LSan with 27 passed, and clang-format/cppcheck/clang-tidy.

Round 1 findings:
- Finding 1 [blocker], UB in `niceStep` for a subnormal `span / maxLines`: **resolved.** `Drawing/gridmath.cpp:62-64` returns early unless `target >= DBL_MIN`. As a result, `floor(log10(target))` is in [-308, 308] before the cast at `:69`. The candidate exponents are -309..311, and `scaled` handles them with no UB: overflow gives inf and is skipped at `:75`, and the low end falls back to `std::pow`. The contract is updated at `Drawing/gridmath.h:21-22`. The tests cover `denorm_min`, `1e-320` with `maxLines = 2^30`, the exact boundary `DBL_MIN/10`, and the first normal target `DBL_MIN*10/10 == DBL_MIN` (`tst_gridmath.cpp:138-143`).
- Finding 2 [major], `gridLines` rounding by `labelDecimals`: **resolved.** `Drawing/gridmath.cpp:113-114` now rounds to `stepDecimals(step)` places.
  - For every `niceStep` output, `stepDecimals` matches the old behaviour. Below the right `d`, `m·10^-k` is in (0, 0.5], so its distance to the nearest integer is far above 8 ulp. At the right `d` (at most 22, where powers of ten are exact), the product is within about 1 ulp of `m`. So the UTM results verified in round 1 are unchanged.
  - For 0.25 and 0.025, it returns 2 and 3, and the new regression tests at `tst_gridmath.cpp:204-209` cover them, including 0.25 at UTM magnitude.
  - The new `|scaled| < 2^53` guard at `:122` prevents a no-op round from damaging large values.
- Finding 3 [nit], the mislabelled `maxCount` case: **resolved.** The overflow case moved to `tst_gridmath.cpp:226-227`, and a real `maxCount` case was added at `:237-238`.

Regressions: none blocking. `stepDecimals` costs at most about 340 cheap iterations and runs once per `gridLines` call. It has no UB paths, because `pow10Positive` overflow and `step * scale` overflow both return -1 before any cast.

### Findings
1. [minor] `Drawing/gridmath.cpp:32-49`: The comment on `stepDecimals` says it returns -1 for a step that is not a terminating decimal, so that such a step is not rounded. The code does not do that.
   - **Why:** The tolerance is `8 * eps * scaled`, which grows with `scaled`. Once `scaled >= 2^47` (about 1.4e14), the tolerance is at least 0.5, so every value passes. So for any normal step, the loop always stops at some `d` where the step has about 15 significant digits, and -1 is returned only for subnormal or overflowing inputs.
   - **Failure scenario:** `gridLines(0, 1, 1.0/7)` gets `d = 15`. The first line becomes `round(142857142857142.84)/1e15 = 0.142857142857143`, which is about 6 ulp away from `step`. `1.0/3` behaves the same way, giving 0.333333333333333, which is off by about 6 ulp. This is numerically small, and Task 02 only passes `niceStep` steps. Still, the comment states a guarantee the code does not provide, and the header (`gridmath.h:27-32`) promises values that are multiples of the step.
   - **Fix:** Make the code match the comment: in the loop, `if (8.0 * eps * scaled >= 0.5) return -1;` before the integer test, because past that point the test is vacuous. Add one test asserting that `gridLines(0, 1, 1.0/3)` returns exactly `(first + i) * step` for each element. Alternatively, keep the code and rewrite the comment to say that non-decimal steps are rounded to about 15 significant digits.
2. [nit] `Drawing/gridmath.h:18-19`: The doc says that for any finite `span > 0`, `span / step` is in `(maxLines / 2.5, maxLines]`. The new `@return` clause at `:21-22` contradicts this for subnormal targets, and so does overflow (`niceStep(1.7e308, 1)` returns 0, as `gridmath.cpp:81` admits). **Fix:** Rephrase the sentence to "if the result is non-zero, span / step lies in (maxLines / 2.5, maxLines]".

### Not reviewed / could not verify
- I could not compile or run my own probes, such as a UBSan run with `float-cast-overflow` or a sweep of `gridLines` over UTM windows for all `niceStep` steps from 1e-9 to 1e7. Local execution needed approval that this non-interactive session could not get. The conclusions above come from reading the code and doing the arithmetic by hand. I relied on the PR-CI run 2 summary and on the coder's report of a local run with `-fsanitize=float-cast-overflow`.
- The PR-CI sanitizer job (`.github/workflows/pr-ci.yml`, tst_gridmath step) uses `-fsanitize=address,undefined`. With GCC that does not include `float-cast-overflow`, so CI would not catch a regression of the round 1 blocker. This is not raised as a finding, because the explicit subnormal test rows now cover that path.
- `niceStep` exactness for subnormal and near-overflow steps (the `std::pow` fallback in `scaled`) is still untested. It is far outside the UTM range, and this is unchanged from round 1.
- I did not open the full PR-CI logs. Changes merged in from main through release/epic-01 are out of scope.
