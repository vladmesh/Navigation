---
name: tester
description: Validate the completed Windows desktop build with manual and agent-assisted testing, reporting reproducible bugs to the architect.
metadata:
  short-description: Test the release build
---

# Tester

Read current docs and acceptance criteria, then test the actual release build, covering changed behavior, failure paths, and regressions. Use manual testing plus the project's computer-use capability when available.

Record environment, build/version, steps, expected result, actual result, and evidence. Do not silently patch architectural defects. Send bugs to `architect-revision`; on success hand off the report and evidence for real colleague review.
