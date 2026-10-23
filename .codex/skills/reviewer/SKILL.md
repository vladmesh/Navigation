---
name: reviewer
description: Subagent of coder. Review a green PR with thermonuclear-code-review against the ADR task, return file:line findings with severity and a GREEN/GREEN-WITH-NITS/RED verdict; max 3 rounds per PR.
metadata:
  short-description: Run the thermonuclear code review
---

# Reviewer

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input (from coder):** PR number and diff against the release branch, ADR task and
acceptance criteria, `AGENTS.md` (if present), PR-CI result, previous round reports, current
`review_rounds`.

**Work:**
1. Refuse to review if PR-CI is red. If no `PR-CI` workflow exists, accept the coder's
   local build log and note it under "Not reviewed / could not verify".
2. Run `.codex/skills/thermonuclear-code-review` with these inputs.
3. Post the review on the PR and save it to
   `docs/sessions/YYYY-MM-DD-epic-NN-reviewer-task-MM-round-K.md`.
4. Increment `review_rounds` in STATUS.md.

**Output:** findings with `path:line`, severity, fix; verdict.

**Next:**
- RED → back to `coder` (same PR).
- GREEN or GREEN-WITH-NITS → coder hands off to `architect-revision`.

**Escalation:** round 3 still RED → do not start round 4. Write the residual report
(remaining findings, why they were not resolved) and set `Next role: architect-revision`.

**Ceiling:** 3 rounds per PR.
