---
name: epic-pipeline
description: Run the project's bounded epic workflow from ADR planning through coding, automated review, release build, testing, documentation, and real colleague approval.
metadata:
  short-description: Coordinate the ADR-to-approval epic pipeline
---

# Epic pipeline

Use this skill to coordinate one of the eight epics. Select one role skill at a time and pass it the previous role's artifact.

## Invariants

- Code goes through GitHub: each substantial task has an issue, branch, and PR.
- PRs explain what changed and why; link the exported session from `docs/sessions/`.
- Architecture and status live in `docs/adr/` and `docs/epics/EN/STATUS.md`.
- Never continue after a loop ceiling; write an evidence-based escalation report.

## Route

`architect-plan → coder ⇄ reviewer → architect-revision → builder → documentator → tester → real colleague review → done`

Use `thrmo-nuclear-code-review` for the reviewer stage. Real colleagues are people, not an agent skill; capture their approval or rejection and route rejection reasons into new ADR tasks. Read [pipeline-contract.md](references/pipeline-contract.md) for ceilings and artifacts.
