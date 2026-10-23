# Epic status

<!-- Copy to docs/epics/EN/STATUS.md at epic start. Every role updates it at hand-off.
     Rules: .codex/skills/epic-pipeline/references/pipeline-contract.md -->

| Field | Value |
|---|---|
| Epic | 01 / 8 — Сетка координат в зоне отрисовки |
| ADR | docs/adr/0001-coordinate-grid.md |
| Release branch | release/epic-01 |
| Stage | release build green (epic-01-rc1) — docs reconciliation |
| Current task | — (01, 02 done) |
| Next role | documentator |
| Hand-off | release-build run 40017826744, artifact Ship-epic-01-rc1 (sha256 d896720e…2b2dda); docs/sessions/2026-10-23-epic-01-builder.md |
| Last update | 2026-10-23 22:20, builder |

## Counters

| Counter | Value | Ceiling | Scope |
|---|---|---|---|
| ci_runs | 0 | 10 | per task |
| review_rounds | 0 | 3 | per PR |
| build_attempts | 1 | 5 | per build cycle |
| colleague_returns | 0 | 2 (3rd → human-epic-owner) | per epic |

## Tasks

| # | Task | Status | Issue | PR |
|---|---|---|---|---|
| 01 | Чистая функция расчёта шага сетки (GridMath) + unit-тест | done | #4 | #5 |
| 02 | Адаптивная сетка и подписи в UberPainter (фикс границ, удаление updateCoordinates) | done | #11 | #12 |

## Log

- 2026-10-23 architect-plan: plan done — ADR 0001 accepted, 2 таска, ветка release/epic-01 создана; конспект docs/sessions/2026-10-23-epic-01-architect-plan.md
- 2026-10-23 coder: task 01 — issue #4, PR #5 в release/epic-01; PR-CI: no workflow yet (ci_runs 0); локально tst_gridmath 27/27 pass (Qt 6.8.3 Linux), Ship.pro собирается; отчёт docs/sessions/2026-10-23-epic-01-coder-task-01.md; next: reviewer
- 2026-10-23 coder: task 01 — release/epic-01 синхронизирована с main (PR #10, PR-CI есть); в PR #5 влита release-ветка, tst_gridmath добавлен в PR-CI; PR-CI run 1
- 2026-10-23 reviewer: task 01 round 1 — RED (1 blocker: UB в niceStep при субнормальном span/maxLines; 1 major: gridLines округляет по labelDecimals, неверно для шага 0.25; 1 nit); PR-CI run 1 зелёный (4/4); отчёт docs/sessions/2026-10-23-epic-01-reviewer-task-01-round-1.md; next: coder
- 2026-10-23 coder: task 01 — исправлены findings 1–3 раунда 1; PR-CI run 2; next: reviewer (round 2)
- 2026-10-23 reviewer: task 01 round 2 — GREEN-WITH-NITS (1 minor, 1 nit); PR-CI run 2 зелёный 4/4; отчёт docs/sessions/2026-10-23-epic-01-reviewer-task-01-round-2.md
- 2026-10-23 coder: task 01 — ниты раунда 2 исправлены, PR-CI run 3; next: architect-revision
- 2026-10-23 architect-revision: task 01 принят — PR-CI run 3 (8d06b21) зелёный 4/4 (tst_gridmath 27/0 на MSVC и ASan), ревью раунд 2 GREEN-WITH-NITS, ниты исправлены; итог по таску: ci_runs 3 (+1 docs-коммит этой передачи), review_rounds 2; PR #5 влит merge-коммитом; ADR Task 01 → done; счётчики сброшены; next: coder (task 02)
- 2026-10-23 coder: task 02 — issue #11, PR #12; локально Ship.pro собирается, offscreen-стенд FAILURES 0; PR-CI run 1; next: reviewer
- 2026-10-23 coder: task 02 — PR-CI run 1 красный (MSVC, Linux ASan: tst_critical не линкуется без gridmath.cpp в Test/app_sources.pri); исправлено, PR-CI run 2
- 2026-10-23 reviewer: task 02 round 1 — GREEN-WITH-NITS (4 minor); PR-CI run 2 зелёный 4/4; отчёт docs/sessions/2026-10-23-epic-01-reviewer-task-02-round-1.md
- 2026-10-23 coder: task 02 — findings 1–2 исправлены (+ фикс UAF в деструкторе, найден стендом под ASan), 3–4 оставлены architect-revision; PR-CI run 3; next: reviewer (round 2)
- 2026-10-23 reviewer: task 02 round 2 — GREEN-WITH-NITS (1 minor perf, 2 перенесённых minor, 1 nit по ADR); отчёт docs/sessions/2026-10-23-epic-01-reviewer-task-02-round-2.md
- 2026-10-23 architect-revision: task 02 принят — итог ci_runs 3 (+1 docs-коммит передачи), review_rounds 2; PR #12 влит merge-коммитом; ADR: Decision §2 и критерий строк уточнены, leftovers записаны; все таски done; счётчики сброшены; next: builder
- 2026-10-23 builder: release-build (workflow_dispatch, run 40017826744) зелёный с 1-й попытки; артефакт Ship-epic-01-rc1, smoke-старт на раннере OK; отчёт docs/sessions/2026-10-23-epic-01-builder.md; next: documentator
