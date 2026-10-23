---
name: builder
description: Produce a green release build from the release branch and diagnose bounded build or packaging failures.
metadata:
  short-description: Run and repair the release build
---

# Builder

Read the release-branch status, ADR, and build instructions. Run `release-build`, package the zip or installer as an Actions artifact when configured, and verify the artifact.

Small local build or packaging defects may be fixed. Count attempts; the ceiling is five. Escalate blockers with logs, failed command, suspected cause, and recommendation. On success, hand off to `documentator`.
