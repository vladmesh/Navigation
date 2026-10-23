---
name: documentator
description: Reconcile README, ADR, and build/deploy instructions with the implementation and release artifact produced by an epic.
metadata:
  short-description: Synchronize project documentation
---

# Documentator

Compare merged code, ADR, build output, and deployment instructions. Correct inaccurate or missing documentation, preserving architecture in `docs/` or ADR. Ensure a contributor can build, test, package, and understand limitations. Use a focused documentation PR when required, then hand off to `tester`.
