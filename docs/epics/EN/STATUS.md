# Epic status

<!-- Copy to docs/epics/EN/STATUS.md at epic start. Every role updates it at hand-off.
     Rules: .codex/skills/epic-pipeline/references/pipeline-contract.md -->

| Field | Value |
|---|---|
| Epic | 01 / 8 — Сетка координат в зоне отрисовки |
| ADR | docs/adr/0001-coordinate-grid.md |
| Release branch | release/epic-01 |
| Stage | planning — done |
| Current task | 01 — Чистая функция расчёта шага сетки + unit-тест (issue —, PR —) |
| Next role | coder |
| Hand-off | docs/adr/0001-coordinate-grid.md, Task 01 |
| Last update | 2026-10-23 18:55, architect-plan |

## Counters

| Counter | Value | Ceiling | Scope |
|---|---|---|---|
| ci_runs | 0 | 10 | per task |
| review_rounds | 0 | 3 | per PR |
| build_attempts | 0 | 5 | per build cycle |
| colleague_returns | 0 | 2 (3rd → human-epic-owner) | per epic |

## Tasks

| # | Task | Status | Issue | PR |
|---|---|---|---|---|
| 01 | Чистая функция расчёта шага сетки (GridMath) + unit-тест | todo | | |
| 02 | Адаптивная сетка и подписи в UberPainter (фикс границ, удаление updateCoordinates) | todo | | |

## Log

- 2026-10-23 architect-plan: plan done — ADR 0001 accepted, 2 таска, ветка release/epic-01 создана; конспект docs/sessions/2026-10-23-epic-01-architect-plan.md
