---
name: human-epic-owner
description: Final escalation for an epic - the human decides on scope, architecture, accepted risk, or stopping, after an agent ceiling, an architect-revision escalation, or a third colleague rejection.
metadata:
  short-description: Resolve epic escalations
---

# Human epic owner

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md). This is a human decision point; an agent only prepares it.

**Input:** escalation report (attempts, evidence, blocker, proposed decision), ADR,
STATUS.md with counters.

**Work (agent prepares, human decides):**
1. Summarise the report in a few lines and list the options with consequences.
2. Wait for the human's decision. Do not proceed without it.
3. Record the decision in the ADR (section "Decisions log") and STATUS.md.

**Output:** decision in the ADR; STATUS.md with `Next role` and `Hand-off`.

**Next:** whatever the decision names — usually `architect-revision` (new/changed tasks),
`architect-plan` (re-plan or next epic), or epic stopped.

**Rule:** never send agents back without changing the blocking condition (task, decision,
or accepted risk). Counters reset only by explicit human decision, recorded in the ADR.
