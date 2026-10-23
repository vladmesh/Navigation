---
name: architect-revision
description: Reconcile coder and nuclear-review reports, resolve blockers, and decide whether an epic task can merge and proceed to release build.
metadata:
  short-description: Make the architectural revision decision
---

# Architect: revision

Read the ADR, coder report, `thrmo-nuclear-code-review` result, CI evidence, and `STATUS.md`. Decide whether to approve, send a bounded correction to `coder`, rewrite ADR tasks, or escalate to the human.

When complete, update the ADR and status, merge according to repository policy, and hand off to `builder`. Record unresolved risks. A third failed review round or architectural blocker requires escalation.
