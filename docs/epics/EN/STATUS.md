# Epic status

<!-- Copy to docs/epics/EN/STATUS.md at epic start. Every role updates it at hand-off.
     Rules: .codex/skills/epic-pipeline/references/pipeline-contract.md -->

| Field | Value |
|---|---|
| Epic | 01 / 8 — Сетка координат в зоне отрисовки |
| ADR | docs/adr/0001-coordinate-grid.md |
| Release branch | release/epic-01 |
| Stage | task 01 — PR open, PR-CI run 1 |
| Current task | 01 — Чистая функция расчёта шага сетки + unit-тест (issue #4, PR #5) |
| Next role | reviewer |
| Hand-off | PR #5 (epic-01/task-01-gridmath → release/epic-01); docs/sessions/2026-10-23-epic-01-coder-task-01.md; ADR 0001 Task 01 |
| Last update | 2026-10-23 21:50, coder |

## Counters

| Counter | Value | Ceiling | Scope |
|---|---|---|---|
| ci_runs | 1 | 10 | per task |
| review_rounds | 0 | 3 | per PR |
| build_attempts | 0 | 5 | per build cycle |
| colleague_returns | 0 | 2 (3rd → human-epic-owner) | per epic |

## Tasks

| # | Task | Status | Issue | PR |
|---|---|---|---|---|
| 01 | Чистая функция расчёта шага сетки (GridMath) + unit-тест | in review | #4 | #5 |
| 02 | Адаптивная сетка и подписи в UberPainter (фикс границ, удаление updateCoordinates) | todo | | |

## Log

- 2026-10-23 architect-plan: plan done — ADR 0001 accepted, 2 таска, ветка release/epic-01 создана; конспект docs/sessions/2026-10-23-epic-01-architect-plan.md
- 2026-10-23 coder: task 01 — issue #4, PR #5 в release/epic-01; PR-CI: no workflow yet (ci_runs 0); локально tst_gridmath 27/27 pass (Qt 6.8.3 Linux), Ship.pro собирается; отчёт docs/sessions/2026-10-23-epic-01-coder-task-01.md; next: reviewer
- 2026-10-23 coder: task 01 — release/epic-01 синхронизирована с main (PR #10, PR-CI есть); в PR #5 влита release-ветка, tst_gridmath добавлен в PR-CI; PR-CI run 1
