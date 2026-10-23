---
name: epic-pipeline
description: Orchestrate one of the eight epics of this Qt/C++ project: read docs/epics/EN/STATUS.md, invoke the next role skill with its hand-off artifact, enforce loop ceilings and escalation.
metadata:
  short-description: Coordinate the ADR-to-approval epic pipeline
---

# Epic pipeline

Coordinates one epic at a time (8 in total). Read
[references/pipeline-contract.md](references/pipeline-contract.md) first: it defines the
route, artifacts, branches, CI names, counters, and ceilings for every role.

## Loop

1. Read `AGENTS.md` (if present) and `docs/epics/EN/STATUS.md`. No STATUS.md yet → copy
   `docs/epics/_template/STATUS.md`, start with `architect-plan`.
2. Invoke the role in `Next role`, passing the `Hand-off` artifact. One role at a time;
   `reviewer` is the only role run as a subagent (of `coder`).
3. After the role finishes, verify it updated STATUS.md (stage, next role, hand-off,
   counters). If not, fix STATUS.md from its report before continuing.
4. If a counter is at its ceiling, do not re-invoke the same role: route to the escalation
   target from the contract.
5. `Next role: colleagues` → stop and ask the human to collect colleague feedback.
   `Next role: human-epic-owner` → stop and present the escalation report.

## Route

`architect-plan → coder ⇄ reviewer → architect-revision → (coder for next task | builder) → documentator → tester → colleagues → epic closed`

Back-edges: builder blocker, tester bugs, colleague rejection → `architect-revision`.
Escalation: architect-revision → `human-epic-owner`; third colleague rejection →
`human-epic-owner`.

## Invariants

- Code goes through GitHub: issue → branch → PR → merge into the release branch.
- PRs say what changed and why and link the session export from `docs/sessions/`.
- Do not claim a CI workflow passed if it does not exist; say "no workflow yet".
- Never continue after a ceiling; write the report instead.
