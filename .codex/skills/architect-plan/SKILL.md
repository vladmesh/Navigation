---
name: architect-plan
description: Turn an epic brief into an actionable ADR, task breakdown, and STATUS handoff for this C/C++ Windows project.
metadata:
  short-description: Plan an epic in an ADR
---

# Architect: plan

Discuss the epic with the human, inspect repository instructions and conventions, and choose an approach with explicit alternatives, risks, and acceptance criteria.

Create `docs/adr/NNNN-epic.md` with independently deliverable tasks, each suitable for its own issue, branch, and PR. Update `docs/epics/EN/STATUS.md` with the current stage, next role (`coder`), exact task, and attempt counters. Export the planning session to `docs/sessions/` when supported.

Do not implement code here. Ask the human about unresolved requirements before handoff.
