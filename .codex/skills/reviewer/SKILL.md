---
name: reviewer
description: Review a green PR with the project's thrmo-nuclear-code-review skill, checking the ADR, implementation, tests, and regressions.
metadata:
  short-description: Run the nuclear code review
---

# Reviewer

Invoke `thrmo-nuclear-code-review` for the review stage. Review only after PR-CI is green. Pass it the ADR, acceptance criteria, diff, tests, relevant surrounding code, and repository instructions.

Return concrete findings with file/line references, severity, and suggested fixes, or an explicit green result. Allow at most three rounds. After the ceiling, send the residual report to `architect-revision`; do not continue the loop.
