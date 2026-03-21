#!/usr/bin/env python3
"""
fix_include_casing.py — Fix wrong-cased #include paths without modifying the paths themselves.

Usage:
    python fix_include_casing.py <root_dir> [<root_dir2> ...] [--dry-run] [--jobs N]

Examples:
    python fix_include_casing.py ./Minecraft.World ./Minecraft.Client
    python fix_include_casing.py ./src --dry-run
    python fix_include_casing.py ./Minecraft.World ./Minecraft.Client --jobs 8

What it does:
  - Indexes every .h/.hpp/.cpp/.cc/.cxx file under all given root directories
  - For each #include "..." in every scanned file:
      1. Resolves the include path relative to the including file
      2. Checks whether the resolved path exists on disk (exact casing)
      3. If not, searches the index for a file whose name matches case-insensitively
      4. Rewrites ONLY the filename component with the correct on-disk casing
         — the rest of the path is left completely untouched
  - Reports unresolvable includes for manual review

Flags:
  --dry-run   Print what would change without writing any files
  --jobs N    Number of parallel workers (default: CPU count)
"""

import os
import re
import sys
import argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
from collections import defaultdict


SOURCE_EXTS = {".h", ".hpp", ".cpp", ".cc", ".cxx"}

INCLUDE_RE = re.compile(r'(#\s*include\s+")([^"]+)"')


# ---------------------------------------------------------------------------
# Indexing
# ---------------------------------------------------------------------------

def build_file_index(root_dirs: list[Path]) -> dict[str, str]:
    """
    Walk all root directories and build:
        lowercase_filename -> real_on_disk_filename   (just the basename, not full path)

    We only need the correct basename spelling — path structure is never touched.
    If two files share a lowercased name but differ in casing, last-write wins;
    that ambiguity is an existing problem in the codebase and not ours to solve here.
    """
    index: dict[str, str] = {}
    for root in root_dirs:
        for _, _, files in os.walk(root):
            for fname in files:
                if Path(fname).suffix.lower() in SOURCE_EXTS:
                    index[fname.lower()] = fname
    return index


def collect_sources(root_dirs: list[Path]) -> list[str]:
    sources = []
    for root in root_dirs:
        for dirpath, _, files in os.walk(root):
            for fname in files:
                if Path(fname).suffix.lower() in SOURCE_EXTS:
                    sources.append(os.path.join(dirpath, fname))
    return sources


# ---------------------------------------------------------------------------
# Per-file processing (runs in worker processes)
# ---------------------------------------------------------------------------

def process_file(
    src: str,
    index: dict[str, str],
    dry_run: bool,
) -> tuple[int, int, list[str]]:
    """
    Scan one file, fix any wrong-cased include basenames, return
    (changes_made, unresolved_count, log_lines).
    """
    try:
        with open(src, "r", encoding="utf-8", errors="replace") as fh:
            lines = fh.readlines()
    except OSError as e:
        return 0, 0, [f"  SKIP (unreadable): {src}: {e}"]

    new_lines = []
    changes = 0
    unresolved = 0
    logs: list[str] = []

    for lineno, line in enumerate(lines, 1):
        stripped = line.lstrip()
        m = INCLUDE_RE.match(stripped)
        if not m:
            new_lines.append(line)
            continue

        inc_path = m.group(2)   # e.g. "../minecraft.client/serverPlayer.h"

        # Resolve to absolute to check real existence
        abs_resolved = os.path.normpath(
            os.path.join(os.path.dirname(src), inc_path.replace("\\", "/"))
        )

        # Already correct — nothing to do
        if os.path.isfile(abs_resolved):
            new_lines.append(line)
            continue

        # Look up just the basename, case-insensitively
        basename = os.path.basename(inc_path)
        real_basename = index.get(basename.lower())

        if not real_basename:
            logs.append(
                f"  UNRESOLVED  {src}:{lineno}\n"
                f"              '{inc_path}'  —  '{basename}' not found in index"
            )
            unresolved += 1
            new_lines.append(line)
            continue

        if real_basename == basename:
            # Basename casing is already correct but file still not found on disk —
            # this is a broken path, not a casing issue; leave it alone
            new_lines.append(line)
            continue

        # Replace only the basename at the end of the path, leave everything else intact
        dir_part = inc_path[: -len(basename)]  # preserves "../Minecraft.Client/net/" exactly
        new_inc_path = dir_part + real_basename

        indent = line[: len(line) - len(stripped)]
        tail_start = stripped.index(inc_path) + len(inc_path) + 1  # past closing "
        tail = stripped[tail_start:]
        new_lines.append(f'{indent}{m.group(1)}{new_inc_path}"{tail}')
        changes += 1

        verb = "WOULD FIX" if dry_run else "FIXED    "
        logs.append(
            f"  {verb}  {src}:{lineno}\n"
            f"              - {inc_path}\n"
            f"              + {new_inc_path}"
        )

    if changes and not dry_run:
        try:
            with open(src, "w", encoding="utf-8") as fh:
                fh.writelines(new_lines)
        except OSError as e:
            logs.append(f"  WRITE ERROR {src}: {e}")
            return 0, unresolved, logs

    return changes, unresolved, logs


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Fix wrong-cased #include filenames without modifying paths.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("roots", nargs="+", help="One or more root directories to scan")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would change without writing files")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4,
                        help="Parallel worker processes (default: CPU count)")
    args = parser.parse_args()

    roots = [Path(r).resolve() for r in args.roots]
    for r in roots:
        if not r.is_dir():
            sys.exit(f"Error: '{r}' is not a directory.")

    print(f"Indexing {len(roots)} root(s) …", flush=True)
    index = build_file_index(roots)
    print(f"  {len(index):>6} unique filenames indexed\n")

    sources = collect_sources(roots)
    print(f"Scanning {len(sources)} source files with {args.jobs} workers …")
    if args.dry_run:
        print("DRY-RUN — no files will be modified.")
    print()

    total_changes = 0
    total_unresolved = 0
    done = 0

    with ProcessPoolExecutor(max_workers=args.jobs) as pool:
        futures = {pool.submit(process_file, src, index, args.dry_run): src for src in sources}
        for future in as_completed(futures):
            changes, unresolved, logs = future.result()
            total_changes += changes
            total_unresolved += unresolved
            done += 1
            for msg in logs:
                print(f"\r{' ' * 60}\r{msg}")
            print(f"\r  {done}/{len(sources)} files …", end="", flush=True)

    print(f"\r{' ' * 60}")
    print("=" * 60)
    verb = "Would fix" if args.dry_run else "Fixed    "
    print(f"  {verb}:   {total_changes} include(s) updated")
    if total_unresolved:
        print(f"  Unresolved: {total_unresolved} include(s) — fix these manually")
    else:
        print(f"  Unresolved: 0  ✓")
    print("=" * 60)


if __name__ == "__main__":
    main()