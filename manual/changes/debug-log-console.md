---
title: Write a debug log every run and open a debug console
category: feature
release: 0.1.0
targets:
- type: command
  id: launch:console-debug
  effect: changed
credit: [ZivDero]
---

Every run now writes a log to a `Debug` folder beside the executable, named for the time the
process started, and keeps two weeks of them. Release builds wrote nothing at all before, so
a crash left nothing to read; debug builds appended to a single `DEBUG.TXT` that the next
launch overwrote. That file is no longer written, and an existing one is left alone.

Each log opens with a banner naming the version, commit, branch, and commit date the build was
made from, whether that working copy had been modified, the time the run started, the Windows
version, and the launch options in use. A log now identifies itself, so a bug report that
includes one no longer has to say which build produced it.

Lines after the banner carry the time they were written. A record that the engine assembles
from several pieces is stamped once, when it starts, rather than once per piece.

The debug console is now a usable window rather than a bare output target. It carries the
scrollback to read what went past, receives ordinary program output such as the `-?` help,
and waits for a keypress when the command line is rejected. Its close button is disabled,
since closing a console window terminates the program that owns it.

Debug builds open the console on every run. Release builds open it with `-XC`, which
previously was rejected outright and stopped startup.

Logs may contain player names and network addresses from multiplayer sessions.
