---
name: computer-use-windows
description: STUB. GUI testing of the Windows release build through a computer-use agent. Not wired up yet; describes the intended procedure and what to do until it exists.
metadata:
  short-description: Computer-use GUI testing (stub)
---

# Computer use on Windows (stub)

> **Status: stub.** No computer-use agent is connected to a Windows host for this project
> yet. Until it is, the tester runs this procedure manually and marks agent-pass cases as
> `not run (no computer-use host)` in the report.

## Intended setup

- A Windows 10/11 machine or VM with the release artifact unpacked.
- A computer-use capable agent (e.g. Claude desktop "Computer use" or Hermes
  `computer_use`) with access to that machine.
- Test inputs: a recorded NMEA log replayed to a virtual COM pair (com0com) or to a local
  TCP port, so tests are repeatable without hardware.

## Procedure

1. Launch `Ship.exe` from the artifact; screenshot the main window.
2. For each test case from the tester's plan: perform the UI steps, screenshot before and
   after, capture the app log (`Service/Logger` output).
3. Check failure paths: open a non-existent COM port, feed malformed NMEA, kill the TCP
   source mid-stream; the app must report the error and stay responsive.
4. Return per case: steps taken, expected vs actual, screenshots, log excerpt.

## Safety

Only touch the app under test and the test input tools. No other windows, no network
settings, no file deletion outside the test folder.
