# Pipeline contract

## Artifacts

- `docs/adr/NNNN-epic.md`: plan, decisions, and task list.
- `docs/epics/EN/STATUS.md`: current stage, next role, handoff artifact, and attempt counters.
- `docs/sessions/`: exported agent sessions, linked from PRs.
- `AGENTS.md`: repository instructions.

## Transitions and ceilings

- Reviewer feedback returns to coder; maximum three review rounds.
- Coding allows at most ten CI runs per task.
- Builder allows at most five attempts.
- Real colleague review may return an epic at most twice; a third return escalates to the human owner.
- Test failures go to architect-revision.
- After approval, close the epic in `STATUS.md` and start the next one.

## CI

PR-CI: Windows/MSVC + CMake, vcpkg binary caching, tests, ASan, and static analysis; add fast LeakSanitizer when Linux builds are supported. Nightly: Windows Dr. Memory. `release-build`: manual on the release branch, publishing a zip or installer artifact.
