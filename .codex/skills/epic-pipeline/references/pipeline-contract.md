# Pipeline contract

Single source of truth for roles, hand-offs, ceilings, and artifacts. Role skills repeat
only their own row; if a role skill and this file disagree, this file wins.

## Artifacts

| Path | Owner | Content |
|---|---|---|
| `AGENTS.md` | everyone reads (if present) | repo rules for agents. Not created by the skills PR; until it exists, the Build section below is authoritative |
| `README.md` | documentator | what the program is, how to build/run/deploy |
| `docs/adr/NNNN-<epic>.md` | architect-plan, architect-revision | decisions, alternatives, risks, task list with acceptance criteria and status per task. Template: `docs/adr/0000-template.md` |
| `docs/epics/EN/STATUS.md` | every role at hand-off | current epic, stage, next role, hand-off artifact, counters. Template: `docs/epics/_template/STATUS.md` |
| `docs/sessions/` | every role | exported agent sessions and role reports, `YYYY-MM-DD-<epic>-<role>-<task>.md`; linked from the PR |

`EN` in `docs/epics/EN/` is the language folder of the status file; there is one STATUS.md
for the epic in progress. Epics are numbered 1..8.

## Build (authoritative until AGENTS.md exists)

qmake only; no CMake, no vcpkg. Qt 6 (MinGW 64-bit or MSVC kit), C++17.

```bat
mkdir build && cd build
qmake ..\Ship.pro CONFIG+=release
mingw32-make -j            :: or nmake / jom with an MSVC kit
:: release\Ship.exe; Ship.pro runs windeployqt as a post-link step on win32 release
```

Qt modules: core gui widgets network serialport location positioning core5compat.
Every new `.cpp/.h/.ui` goes into `Ship.pro`. `Test/` holds hand-written check classes
compiled into the main binary; if a separate test project exists, build it as `PR-CI` does.

## Branches

- `main`: last approved state.
- Release branch: `release/epic-NN` (name recorded in STATUS.md). Created by
  `architect-plan` from `main` at epic start. All task PRs target it.
- Task branch: `epic-NN/task-MM-<slug>` from the release branch, one per ADR task.
- After colleague approval `architect-revision` merges the release branch into `main`.

## CI (by workflow name; maintained by a separate agent in `.github/workflows/`)

- `PR-CI`: runs on every PR. Its exact jobs are whatever the workflow file defines; read it,
  do not assume.
- `nightly`: scheduled heavier checks.
- `release-build`: manual (`workflow_dispatch`) on the release branch, produces the
  Windows artifact (zip/installer).

Any of these may not exist yet. If a workflow is missing, the role says so in its report,
substitutes the local qmake build from the Build section, and does not claim CI is green.
Never edit `.github/` from a pipeline role unless the ADR task is explicitly about CI.

## Route

```
architect-plan
   │ ADR with tasks
   ▼
coder ⇄ reviewer (thermonuclear-code-review, subagent of coder)
   │ green-reviewed PR + coder/reviewer reports
   ▼
architect-revision ── tasks left ──► coder (next task)
   │ all tasks merged into release branch
   ▼
builder ── architectural blocker ──► architect-revision
   │ green release-build
   ▼
documentator
   │ build + reconciled docs
   ▼
tester ── bugs ──► architect-revision (new tasks)
   │ verified build
   ▼
colleagues (humans) ── reject ──► architect-revision (new tasks)
   │ approve
   ▼
epic closed → architect-plan for next epic (until 8)
```

## Role table

| Role | Input | Output | Next on success | Escalation | Ceiling |
|---|---|---|---|---|---|
| architect-plan | epic brief, human discussion | ADR with tasks, release branch, STATUS | coder (task 1) | human-epic-owner (unclear scope) | — |
| coder | ADR task, STATUS | issue, branch, PR with green PR-CI, coder report | reviewer (same PR) | architect-revision | 10 PR-CI runs per task |
| reviewer | green PR, ADR task | thermonuclear review: findings + verdict | coder (if RED); architect-revision (GREEN / GREEN-WITH-NITS) | architect-revision | 3 review rounds per PR |
| architect-revision | coder + reviewer reports, PR, CI logs; or builder/tester/colleague report | merged PR, updated ADR | coder (tasks left) or builder (all done) | human-epic-owner | — (bounded by others' ceilings) |
| builder | release branch with all tasks merged | green release-build artifact, build report | documentator | architect-revision | 5 build attempts |
| documentator | green artifact, ADR, merged code | reconciled README/ADR/build+deploy docs | tester | architect-revision | — |
| tester | artifact + docs + acceptance criteria | test report | colleagues | architect-revision (bugs) | — |
| colleagues (humans) | verified build + test report | approve / reject with reasons | epic closed | architect-revision (reject) | 2 returns per epic |
| human-epic-owner | escalation report | decision recorded in ADR | role named in decision | — | — |

## Counters (in STATUS.md, reset rules)

- `ci_runs` per task: incremented on each PR-CI run the coder triggers; reset on next task.
- `review_rounds` per PR: incremented on each reviewer verdict; reset on next task.
- `build_attempts` per epic build cycle: reset when builder is entered after new tasks.
- `colleague_returns` per epic: never reset within an epic.

## Ceiling rule

When a counter reaches its ceiling, the role **stops**. It does not start another
attempt, does not "just try one more thing", and does not hand off to the success path.
It writes a report to `docs/sessions/` and the escalation target named in the table, and
sets STATUS.md `Next role` to that target. Report content:

- what was attempted (each attempt, one line) and the evidence (CI links, logs, commits);
- the current blocker and suspected root cause;
- a proposed decision (change task, split task, accept risk, change architecture).

Escalation chain: role → architect-revision → human-epic-owner. A third colleague
rejection goes straight to human-epic-owner.

## Hand-off protocol (every role, at the end)

1. Commit role artifacts (report in `docs/sessions/`, ADR/STATUS edits).
2. Update STATUS.md: `Stage`, `Next role`, `Hand-off`, counters, `Last update`.
3. Final message names the next role and the exact artifact path/PR to pass.

The orchestrator (`epic-pipeline`) reads STATUS.md to decide whom to invoke next.
