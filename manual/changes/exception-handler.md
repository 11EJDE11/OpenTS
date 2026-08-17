---
title: Report a crash with a minidump and a readable report
category: feature
release: 0.1.0
targets:
- type: command
  id: launch:no-exception-trap
  effect: removed
- type: command
  id: launch:exception-test
  effect: added
---

A crash now writes everything needed to diagnose it into a folder of its own, named for the
time it happened, under an `Exceptions` folder beside the executable. The folder holds a
minidump, a readable report, and a copy of the end of that run's debug log. Reporting a crash
means attaching that one folder.

The report names the fault and explains what it means, gives the address the code was reading
or writing, and identifies the crash site by function, source file, and line. Two separate
call stacks follow, one read from the saved frame pointers and one reconstructed from the
symbol file, so a stack too damaged for the second still yields the first. The registers, the
bytes at the faulting instruction, the loaded modules with the address range each occupies,
the memory position, and a scan of the stack follow.

Addresses now carry names. The engine ships a symbol file next to the executable, and the
handler is pointed at that directory rather than at whatever folder the game happened to be
launched from, so a report from a player's machine reads the same as one from a developer's.
A report made without a usable symbol file still identifies every address by module and
offset.

The minidump opens on the thread that actually crashed. It can be loaded in a debugger to
inspect the state of the process at the moment of the fault. The crash dialog also offers to
save a second, much larger dump containing the whole address space, for the crashes a summary
cannot explain.

Crashes that previously went unreported now produce a report:

- A crash on any thread. Only crashes on the main thread were reported before; a fault in the
  sound or video callbacks ended the process silently.
- A crash during startup, from the first instruction of the program. The handler used to be
  installed only after the window, sound, and renderer were already running.
- A stack overflow, a call to a pure virtual function, a rejected argument to a runtime
  function, and an aborted run. None of these were reported at all before.
- An unrecoverable engine error. These previously wrote their message to a place a windowed
  program has no way of showing, so nothing appeared on screen.

The dialog itself now shows the report rather than a fixed apology, and gives the folder the
crash was saved to. The old dialog referred players to a support address that no longer
exists and, because of a mismatch between the dialog and the code filling it in, never
managed to display any of the detail it had collected.

Artifact folders older than thirty days are deleted at startup. This is separate from the
debug log's own two weeks, since a crash folder is worth keeping longer than an ordinary run.
The copied portion of the log is limited to the last 256 KiB, and may contain player names and
network addresses like any other log; see [Debug logging](/using/debug-logging/) for what a
log records and what to consider before sharing one.

`-XE` has been removed. It existed to leave a crash to an attached debugger instead of the
engine, and that now happens on its own: a debugger sees a crash before this handler does,
without being asked.

`-EXCEPTIONTEST=<fault>` is new. It raises a chosen fault deliberately so that crash
reporting can be checked on a given machine without waiting for a real crash.
