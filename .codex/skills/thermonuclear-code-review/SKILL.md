---
name: thermonuclear-code-review
description: Run the strictest possible review of a green PR in this Qt6/C++17 Windows project: correctness (UB, ownership, Qt parent trees, signals/slots, threads, parsing of NMEA/COM/TCP input), structure, and ADR acceptance criteria. Returns file:line findings with severity and a GREEN/RED verdict. Used by the reviewer role.
metadata:
  short-description: Thermonuclear C++/Qt code review
---

# Thermonuclear code review (C++/Qt)

Adapted from the owner's `thermo-nuclear-code-quality-review`
(`vladmesh/ummanu`, `skills/roles/ummanu/thermo-nuclear-code-quality-review/SKILL.md`,
commit `ad286ae0`, 2026-10-02). The original is language-agnostic and only covers
structure and maintainability. This version keeps that bar and adds C/C++/Qt correctness,
Windows/MSVC specifics, and the epic's ADR acceptance criteria.

The review is deliberately harsh. "It compiles and CI is green" is the entry condition,
not the approval bar.

## Input (from `reviewer`)

- PR number/branch and the full diff against the release branch (`git diff <release>...HEAD`).
- ADR `docs/adr/NNNN-*.md`: the task being reviewed and its acceptance criteria.
- `AGENTS.md` (if present), `docs/epics/EN/STATUS.md` (current round number), previous round reports.
- PR-CI result (link or "no workflow yet" plus the local build log).

Read the surrounding code of every touched function, not only the hunk. For a header
change, read every includer that the diff relies on.

## Review passes (all of them, in this order)

1. **ADR conformance.** Does the diff implement exactly the assigned task? Every acceptance
   criterion: met / not met / not verifiable, with evidence. Scope creep or a silent change
   of an ADR decision is a blocker.
2. **Correctness and safety in C++/Qt.** Walk the checklist in
   [references/cpp-qt-checklist.md](references/cpp-qt-checklist.md): UB, ownership and raw
   pointers, Qt parent ownership, signals/slots and connection types, threads and races,
   exceptions, integer overflow and conversions, parsing of external data (NMEA, sockets,
   COM ports, files), RAII, Windows/MSVC specifics.
3. **Structure (from the original skill).** Look for a "code judo" move that deletes
   complexity instead of moving it. Flag: a file pushed from under 1000 lines to over 1000
   (`mainwindow.cpp` is already ~1000); ad-hoc conditionals bolted into unrelated flows;
   one-off flags and nullable modes; feature logic leaking into shared modules
   (`Connections/`, `Service/`, `DataVault/`); thin wrappers; copy-paste instead of a
   helper; logic in the wrong layer (UI code doing parsing or coordinate math, parsers
   touching widgets); non-atomic state updates.
4. **Tests and evidence.** New parsing or math logic needs a check that runs (the current
   `Test/` classes are hand-written and compiled into the main binary; a new check must at
   least be callable and documented in the PR). Hard-coded developer paths
   (`C:/Users/...`) in new code are a blocker.
5. **Build hygiene.** New `.cpp/.h/.ui` files listed in `Ship.pro`; no new compiler warnings
   that the diff introduces; no dead code added under `#if 0` or commented-out blocks;
   no change to `.github/` unless the task is about CI.

## Severity

- **blocker**: UB, memory/ownership bug, data race, crash on malformed input, unmet
  acceptance criterion, ADR violation, build break on MSVC or MinGW, structural regression
  listed as a presumptive blocker below.
- **major**: real defect or maintainability regression that will bite soon, but no
  immediate crash or data loss (missing error handling on a socket, wrong connection
  type, file crosses 1000 lines, duplicated logic).
- **minor**: local clarity or robustness issue; fix in this PR if cheap.
- **nit**: naming, formatting, comments. Never more than a handful; do not flood.

Presumptive blockers from the original skill (author must justify or fix): a plausible
code-judo move that deletes incidental complexity is ignored; a file goes from below 1000
lines to above; ad-hoc branching tangles an existing flow; feature checks scattered across
shared code; an unnecessary wrapper or cast-heavy contract; duplication of an existing
helper or logic in the wrong layer.

## Output (write to the PR as a review comment and to `docs/sessions/` report)

```
## Thermonuclear review — PR #<n>, round <k>/3
Verdict: GREEN | GREEN-WITH-NITS | RED

### Acceptance criteria
- [x] <criterion> — evidence: <file:line / test / CI log>
- [ ] <criterion> — why not met

### Findings
1. [blocker] Parsers/nmeaparser.cpp:57 — <what is wrong> → <concrete fix>
2. [major] ...
3. [minor] ...

### Not reviewed / could not verify
- <e.g. no PR-CI workflow yet, built locally with qmake+MinGW only>
```

Every finding has `path:line`, severity, the failure scenario (input or interleaving that
breaks it), and a concrete fix. No "maybe consider".

## Verdict rules

- **RED**: at least one blocker or major.
- **GREEN-WITH-NITS**: only minor/nit remain. The loop stops here: the coder may fix them,
  but no new review round is required.
- **GREEN**: nothing to fix.

Give the full set of blockers in one round. Do not keep a blocker in reserve for the next
round, and do not widen scope once the author has answered the round you already gave.
In round k>1 check only: previous findings fixed, no regressions introduced by the fixes.

## Tone

Direct, serious, demanding, not rude. If the change makes the codebase messier or less
safe, say so plainly. Prefer a few high-conviction findings over a long cosmetic list.
