---
name: architect-plan
description: Discuss an epic with the human and turn it into docs/adr/NNNN-<epic>.md with independently deliverable tasks and acceptance criteria, create the release branch, and hand off the first task to coder.
metadata:
  short-description: Plan an epic in an ADR
---

# Architect: plan

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input:** epic brief from the human (or from `epic-pipeline`), `AGENTS.md` (if present), `README.md`,
previous ADRs, the code.

**Work:**
1. Discuss the epic with the human: goal, constraints, what "done" means for colleagues.
   Ask about every unresolved requirement before writing tasks. Do not implement code.
2. Inspect the affected modules (`Connections/`, `Parsers/`, `Service/`, ...) and note the
   real constraints: qmake `Ship.pro`, Qt6/C++17, Windows target, existing `Test/` classes.
3. Write `docs/adr/NNNN-<epic>.md` from `docs/adr/0000-template.md`: context, decision,
   alternatives, risks, and tasks. Each task: one issue/branch/PR, acceptance criteria that
   a reviewer and tester can check, status `todo`.
4. Create the release branch `release/epic-NN` from `main` and push it.
5. Create or reset `docs/epics/EN/STATUS.md` from `docs/epics/_template/STATUS.md`.
6. Export the planning session to `docs/sessions/`.

**Output:** ADR (status `accepted`), release branch, STATUS.md.

**Next:** `coder`, hand-off = ADR path + task 1 id.

**Escalation:** requirements the human cannot settle, or scope larger than one epic →
`human-epic-owner`.

**Ceiling:** none; do not hand off while open questions remain.
