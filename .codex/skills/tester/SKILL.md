---
name: tester
description: Test the release artifact of this Windows Qt app manually and with a computer-use agent against ADR acceptance criteria; bugs go to architect-revision, a verified build goes to colleagues.
metadata:
  short-description: Test the release build
---

# Tester

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input:** artifact link and build report, reconciled docs, ADR acceptance criteria.

**Work:**
1. Build a test plan from acceptance criteria: changed behaviour, failure paths
   (disconnected COM port, malformed NMEA, dropped TCP), regressions in neighbouring
   features.
2. Manual pass: the human (or tester on a Windows host) runs the plan.
3. Agent pass: GUI checks via `.codex/skills/computer-use-windows` (currently a stub —
   read it for what is and is not available).
4. Record for every case: environment, build/version, steps, expected, actual, evidence
   (screenshot/log). Save the report to `docs/sessions/`.

**Output:** test report: pass/fail per criterion, bug list.

**Next:**
- all criteria pass → `colleagues` (humans), hand-off = artifact + test report. Set
  `Next role: colleagues` and stop.
- bugs → `architect-revision`, hand-off = bug report. Do not patch code yourself.

**Escalation:** cannot test (no Windows host, no artifact) → `architect-revision` with the
reason.

**Ceiling:** none of its own; repeated returns are bounded by builder/coder ceilings.
