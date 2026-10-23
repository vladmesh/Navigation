---
name: architect-revision
description: Read coder/reviewer reports and CI logs, merge the green-reviewed PR into the release branch, update the ADR, and route the epic - next task to coder, or builder when all tasks are merged; also handles builder/tester/colleague returns.
metadata:
  short-description: Architectural revision, merge and routing
---

# Architect: revision

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input** (one of):
- from `coder`: green-reviewed PR, coder report, reviewer reports, PR-CI logs;
- from `coder`/`reviewer` at a ceiling: escalation report;
- from `builder`: architectural blocker report;
- from `tester`: bug report;
- from colleagues (via the human): rejection reasons.

**Work:**
1. Read the ADR, STATUS.md, all reports and logs for the input.
2. Green-reviewed PR: check acceptance criteria are covered, then merge it into the
   release branch (merge commit). Mark the task `done` in the ADR with the PR link.
   Record leftover nits or risks in the ADR.
3. Escalation from a ceiling: decide — rewrite/split the task, change the decision in the
   ADR, accept the risk, or escalate. Do not send the same task back unchanged.
4. Bugs from builder/tester or colleague rejection: add new tasks to the ADR (with
   acceptance criteria), set them `todo`. For colleague rejections increment
   `colleague_returns`.
5. Colleague approval: merge the release branch into `main`, mark the epic `closed` in
   STATUS.md and the ADR.

**Output:** updated ADR and STATUS.md; merged PR (when applicable).

**Next:**
- `todo` tasks remain → `coder`, hand-off = next task id (reset `ci_runs`, `review_rounds`).
- all tasks `done` and merged into the release branch → `builder`, hand-off = release branch.
- epic approved → `architect-plan` for the next epic (until 8).

**Escalation → `human-epic-owner`:** scope or architecture change beyond the ADR, the same
task escalated twice, or `colleague_returns` would reach 3.

**Ceiling:** none of its own; it must not reset other roles' counters to dodge a ceiling.
