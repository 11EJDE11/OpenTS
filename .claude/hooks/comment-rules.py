#!/usr/bin/env python3
"""Re-state the comment rules when an agent edits engine source.

Claude Code runs this before every Edit and Write. When the target is a C or
C++ file under code/, it returns the comment sections of code/AGENTS.md as
additional context, so the rules are present at the moment of the edit rather
than only at the start of the session. code/AGENTS.md stays the single owner;
this script never carries its own copy of the rules.

Any failure exits quietly. A missing rule reminder is better than a blocked
edit.
"""

import json
import sys
from pathlib import Path

SECTIONS = ("Comments", "Comment styles")
SUFFIXES = (".h", ".hpp", ".hh", ".inl", ".c", ".cpp", ".cc")

ROOT = Path(__file__).resolve().parent.parent.parent
RULES = ROOT / "code" / "AGENTS.md"


def targets_engine_source(payload):
    path = (payload.get("tool_input") or {}).get("file_path")
    if not path:
        return False
    try:
        resolved = Path(path).resolve()
    except OSError:
        return False
    if resolved.suffix.lower() not in SUFFIXES:
        return False
    return (ROOT / "code") in resolved.parents


def comment_sections(text):
    """Return the SECTIONS headings of code/AGENTS.md and their bodies."""
    kept = []
    keeping = False
    for line in text.splitlines():
        if line.startswith("## "):
            keeping = line[3:].strip() in SECTIONS
        elif line.startswith("# "):
            keeping = False
        if keeping:
            kept.append(line)
    return "\n".join(kept).strip()


def main():
    try:
        payload = json.load(sys.stdin)
    except (json.JSONDecodeError, ValueError):
        return
    if not targets_engine_source(payload):
        return

    try:
        rules = comment_sections(RULES.read_text(encoding="utf-8"))
    except OSError:
        return
    if not rules:
        return

    json.dump(
        {
            "suppressOutput": True,
            "hookSpecificOutput": {
                "hookEventName": "PreToolUse",
                "additionalContext": (
                    "Comment rules for code/, from code/AGENTS.md. They govern"
                    " this edit even where the surrounding lines disagree.\n\n"
                    + rules
                ),
            },
        },
        sys.stdout,
    )


if __name__ == "__main__":
    main()
