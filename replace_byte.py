#!/usr/bin/env python3
"""
Recursively replaces the custom `byte` type alias with `uint8_t` in C++ source files.

Usage:
    python replace_byte.py [root_dir] [--dry-run] [--extensions .cpp .h .hpp ...]

Defaults:
    root_dir     = current working directory
    extensions   = .cpp, .cc, .cxx, .h, .hpp, .hxx
"""

import re
import sys
import argparse
from pathlib import Path

# Matches `byte` only when it appears as a standalone word (not as part of
# another identifier, e.g. "byteswap" or "mbyte" are left untouched).
# Also avoids touching `std::byte` — the very thing we're migrating toward.
BYTE_RE = re.compile(r'(?<!:)(?<!\w)\bbyte\b(?!\w)')

DEFAULT_EXTENSIONS = {".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx"}

# ---------------------------------------------------------------------------
# Comment-aware tokeniser
# Splits a C++ source file into a flat list of (kind, text) spans where kind
# is one of:
#   "code"         — real code (substitution applies here)
#   "line_comment" — // … \n
#   "block_comment"— /* … */
#   "string"       — "…" or '…' literals (left untouched too)
# ---------------------------------------------------------------------------
_TOKEN_RE = re.compile(
    r'(?P<block_comment>/\*.*?\*/)'        # /* ... */
    r'|(?P<line_comment>//[^\n]*)'         # // ...
    r'|(?P<string>"(?:\\.|[^"\\])*"'       # "string"
        r"|'(?:\\.|[^'\\])*')"             # 'char'
    r'|(?P<code>[^/\'"]+|/)',              # everything else (including lone /)
    re.DOTALL,
)

def _tokenise(src: str) -> list[tuple[str, str]]:
    return [(m.lastgroup, m.group()) for m in _TOKEN_RE.finditer(src)]


def process_file(path: Path, dry_run: bool) -> tuple[int, list[str]]:
    """Return (replacement_count, list_of_changed_lines_summary)."""
    original = path.read_text(encoding="utf-8", errors="replace")

    tokens = _tokenise(original)

    # Apply substitution only to "code" spans; track per-line changes.
    result_chars: list[str] = []
    count = 0

    for kind, text in tokens:
        if kind == "code":
            new_text, n = BYTE_RE.subn("uint8_t", text)
            count += n
            result_chars.append(new_text)
        else:
            result_chars.append(text)

    new_source = "".join(result_chars)

    # Build human-readable per-line diff for changed lines only.
    changes: list[str] = []
    if count:
        old_lines = original.splitlines()
        new_lines = new_source.splitlines()
        for lineno, (old, new) in enumerate(zip(old_lines, new_lines), start=1):
            if old != new:
                changes.append(
                    f"  line {lineno:>5}: {old.rstrip()!r}  ->  {new.rstrip()!r}"
                )

    if count and not dry_run:
        path.write_text(new_source, encoding="utf-8")

    return count, changes


def main():
    parser = argparse.ArgumentParser(
        description="Replace custom `byte` type with `uint8_t` in a C++ codebase."
    )
    parser.add_argument(
        "root_dir",
        nargs="?",
        default=".",
        help="Root directory to search (default: current directory)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Preview changes without writing any files",
    )
    parser.add_argument(
        "--extensions",
        nargs="+",
        default=None,
        metavar="EXT",
        help=(
            "File extensions to process, e.g. --extensions .cpp .h "
            f"(default: {' '.join(sorted(DEFAULT_EXTENSIONS))})"
        ),
    )
    args = parser.parse_args()

    root = Path(args.root_dir).resolve()
    if not root.is_dir():
        print(f"Error: '{root}' is not a directory.", file=sys.stderr)
        sys.exit(1)

    extensions = (
        {e if e.startswith(".") else f".{e}" for e in args.extensions}
        if args.extensions
        else DEFAULT_EXTENSIONS
    )

    if args.dry_run:
        print("=== DRY RUN — no files will be modified ===\n")

    total_files = 0
    total_replacements = 0

    for path in sorted(root.rglob("*")):
        if path.suffix not in extensions or not path.is_file():
            continue

        try:
            count, changes = process_file(path, dry_run=args.dry_run)
        except (OSError, UnicodeDecodeError) as exc:
            print(f"[SKIP] {path}: {exc}", file=sys.stderr)
            continue

        if count:
            total_files += 1
            total_replacements += count
            status = "would update" if args.dry_run else "updated"
            print(f"[{status}] {path}  ({count} replacement{'s' if count != 1 else ''})")
            for line in changes:
                print(line)
            print()

    print("─" * 60)
    if args.dry_run:
        print(
            f"Dry run complete. Would replace {total_replacements} occurrence(s) "
            f"across {total_files} file(s)."
        )
    else:
        print(
            f"Done. Replaced {total_replacements} occurrence(s) "
            f"across {total_files} file(s)."
        )


if __name__ == "__main__":
    main()