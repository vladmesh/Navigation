---
name: builder
description: Produce a green Windows release build of the release branch via the release-build workflow (or local qmake if absent), max 5 attempts; fix small build defects itself, send architectural blockers to architect-revision, hand the green artifact to documentator.
metadata:
  short-description: Run and repair the release build
---

# Builder

Contract: [pipeline-contract](../epic-pipeline/references/pipeline-contract.md).

**Input:** release branch with all ADR tasks merged (STATUS.md `Hand-off`), `AGENTS.md` (if present).

**Work:**
1. Trigger `release-build` (`workflow_dispatch` on the release branch) and download the
   artifact. If the workflow does not exist yet, build locally per the Build section of the contract
   (qmake `Ship.pro` → release → `windeployqt`), zip the output folder, and record that
   it is a local build.
2. Smoke-check the artifact: the `.exe` and Qt DLLs/plugins (`platforms/qwindows.dll`,
   serialport, network, positioning) are present; the app starts if a Windows host is
   available.
3. Each attempt increments `build_attempts`. Small defects (missing file in `Ship.pro`,
   include, deploy step, warning-as-error) → fix on a `epic-NN/build-fix-<slug>` branch
   with a PR into the release branch, then retry.
4. Write the build report to `docs/sessions/`: attempts, commands, artifact name/link, hash.

**Output:** green build artifact + build report.

**Next:** `documentator`, hand-off = artifact link + build report.

**Escalation → `architect-revision`:** defect needs a design change or touches task
logic; or the ceiling is hit.

**Ceiling:** 5 attempts. At the 5th failure stop and write the ceiling report.
