---
name: documentator
description: Reconcile README.md, the epic ADR, and build/deploy instructions with the merged code and the green release artifact; hand build + docs to tester.
metadata:
  short-description: Synchronize project documentation
---

# Documentator

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input:** build artifact + build report from `builder`, ADR, release branch.

**Work:**
1. Compare `README.md`, `AGENTS.md` (if present), and the ADR with what was actually merged and built:
   features, settings, ports and protocols (NMEA inputs, `Sender` TCP JSON), file
   locations, known limitations.
2. Check build/deploy instructions by the builder's real commands (qmake, Qt version,
   `windeployqt`, artifact layout). Fix anything stale or missing.
3. Mark in the ADR what was documented where.
4. Changes go in a docs PR into the release branch (merge it yourself if docs-only and
   PR-CI is green or absent; otherwise ask architect-revision).

**Output:** reconciled docs merged into the release branch; list of changes.

**Next:** `tester`, hand-off = artifact link + docs commit/PR + acceptance criteria.

**Escalation → `architect-revision`:** code behaviour contradicts the ADR (that is a bug,
not a docs fix).

**Ceiling:** none; one pass per build.
