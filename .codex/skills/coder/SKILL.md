---
name: coder
description: Implement one ADR task of this Qt/C++ project via issue → branch → PR into the release branch, drive PR-CI to green (max 10 runs), then run the reviewer subagent (max 3 rounds) and hand the green-reviewed PR to architect-revision.
metadata:
  short-description: Implement one ADR task through a PR
---

# Coder

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input:** ADR task id from STATUS.md `Hand-off`, the ADR, `AGENTS.md` (if present).

**Work:**
1. Open a GitHub issue for the task (title = ADR task, body = acceptance criteria + ADR link).
2. Branch `epic-NN/task-MM-<slug>` from the release branch.
3. Implement only this task. New files go into `Ship.pro`. Build locally with qmake
   (Build section of the contract) when a Qt toolchain is available.
4. Open a PR into the release branch: what changed, why, how it was checked, link to the
   issue, the ADR task, and the session export in `docs/sessions/`.
5. Drive `PR-CI` to green. Each run increments `ci_runs` in STATUS.md. If the `PR-CI`
   workflow does not exist yet, record "no PR-CI workflow" and attach the local build log
   instead; do not call it green.
6. With a green PR, run `reviewer` as a subagent. RED → fix the findings, push (CI again,
   counts toward `ci_runs`), re-run reviewer. Stop when the verdict is GREEN or
   GREEN-WITH-NITS (nits may be fixed without a new round).
7. Write the coder report to `docs/sessions/`: commits, CI runs, review rounds, open nits.

**Output:** PR with green PR-CI (or documented absence) and GREEN/GREEN-WITH-NITS review;
coder report; reviewer reports.

**Next:** `architect-revision`, hand-off = PR number + report paths. The coder does not
merge.

**Escalation:** to `architect-revision` with the ceiling report from the contract when the
task is blocked by architecture or by an ADR mistake.

**Ceilings:** 10 PR-CI runs per task; 3 review rounds per PR. At either ceiling stop
immediately and escalate; do not start run 11 or round 4.
