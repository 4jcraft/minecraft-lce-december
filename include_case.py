#!/usr/bin/env python3
"""
fix_includes.py — Fix cross-package relative #include paths after a refactor.

Usage:
    python fix_includes.py <package_a_dir> <package_b_dir> [--dry-run] [--jobs N]

Examples:
    python fix_includes.py ./Minecraft.World ./Minecraft.Client
    python fix_includes.py ./Minecraft.World ./Minecraft.Client --dry-run
    python fix_includes.py ./Minecraft.World ./Minecraft.Client --jobs 8

What it does:
  - Recursively scans all .h/.hpp/.cpp/.cc/.cxx files in both packages
  - Detects #include "..." paths that cross into the OTHER package
    (identified by the other package's directory name appearing in the path)
  - Finds the file's new location in the other package's current layout
  - If multiple files share the same name, picks the one with the shortest
    relative path from the including file (fewest ../ hops)
  - Rewrites the include in-place with the corrected relative path
  - Reports any includes it could NOT resolve so you can fix them manually

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

# Matches:  #include "some/../path/File.h"
# Captures the full include path in group 1
INCLUDE_RE = re.compile(r'(#\s*include\s+")([^"]+)"')


# ---------------------------------------------------------------------------
# Indexing
# ---------------------------------------------------------------------------

def build_file_index(package_dir: Path) -> dict[str, list[str]]:
    """filename.lower() -> [list of absolute paths] for every header/source in the tree.
    Keys are lowercased so all lookups are case-insensitive (handles MSVC heritage)."""
    index: dict[str, list[str]] = defaultdict(list)
    for root, _, files in os.walk(package_dir):
        for fname in files:
            if Path(fname).suffix in SOURCE_EXTS:
                index[fname.lower()].append(os.path.join(root, fname))
    return dict(index)


def collect_sources(package_dir: Path) -> list[str]:
    sources = []
    for root, _, files in os.walk(package_dir):
        for fname in files:
            if Path(fname).suffix in SOURCE_EXTS:
                sources.append(os.path.join(root, fname))
    return sources


# ---------------------------------------------------------------------------
# Path helpers
# ---------------------------------------------------------------------------

def rel_path_to(from_file: str, to_file: str) -> str:
    """Shortest relative path from from_file's directory to to_file, forward slashes."""
    rel = os.path.relpath(to_file, os.path.dirname(from_file))
    rel = rel.replace(os.sep, "/")
    if not rel.startswith("."):
        rel = "./" + rel
    return rel


def best_match(candidates: list[str], from_file: str) -> str:
    """Pick the candidate with the fewest path components from from_file."""
    return min(candidates, key=lambda c: rel_path_to(from_file, c).count("/"))


# ---------------------------------------------------------------------------
# Per-file processing (runs in worker processes — must be pickle-able)
# ---------------------------------------------------------------------------

def process_file(
    src: str,
    other_pkg_name: str,
    other_index: dict[str, list[str]],
    dry_run: bool,
) -> tuple[int, int, list[str]]:
    """
    Scan one source file, fix cross-package includes, return
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

        inc_path = m.group(2)                   # e.g. "../Minecraft.Client/net/ServerPlayer.h"
        parts = Path(inc_path.replace("\\", "/")).parts

        # Only touch includes that reference the other package by its folder name
        # Case-insensitive: MSVC builds often have wrong casing in include paths
        if other_pkg_name.lower() not in [p.lower() for p in parts]:
            new_lines.append(line)
            continue

        basename = parts[-1]                    # "ServerPlayer.h"
        candidates = other_index.get(basename.lower())

        if not candidates:
            logs.append(
                f"  UNRESOLVED  {src}:{lineno}\n"
                f"              '{inc_path}'  —  '{basename}' not found in {other_pkg_name}"
            )
            unresolved += 1
            new_lines.append(line)
            continue

        new_rel = rel_path_to(src, best_match(candidates, src))

        if new_rel == inc_path:
            new_lines.append(line)
            continue

        # Rebuild the line, preserving leading whitespace + everything after closing "
        indent = line[: len(line) - len(stripped)]
        # Everything after the closing quote of the include
        tail_offset = stripped.index(m.group(2)) + len(m.group(2)) + 1  # +1 for closing "
        tail = stripped[tail_offset:]
        new_line = f'{indent}{m.group(1)}{new_rel}"{tail}'
        new_lines.append(new_line)
        changes += 1

        verb = "WOULD FIX" if dry_run else "FIXED    "
        logs.append(
            f"  {verb}  {src}:{lineno}\n"
            f"              - {inc_path}\n"
            f"              + {new_rel}"
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
        description="Fix cross-package relative #includes after a refactor.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("pkg_a", help="First package dir  (e.g. ./Minecraft.World)")
    parser.add_argument("pkg_b", help="Second package dir (e.g. ./Minecraft.Client)")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would change without writing files")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4,
                        help="Parallel worker processes (default: CPU count)")
    args = parser.parse_args()

    pkg_a = Path(args.pkg_a).resolve()
    pkg_b = Path(args.pkg_b).resolve()

    for p in (pkg_a, pkg_b):
        if not p.is_dir():
            sys.exit(f"Error: '{p}' is not a directory.")

    # --- Index both packages ------------------------------------------------
    print(f"Indexing {pkg_a.name} …", flush=True)
    index_a = build_file_index(pkg_a)
    print(f"  {sum(len(v) for v in index_a.values()):>6} files  ({len(index_a)} unique names)")

    print(f"Indexing {pkg_b.name} …", flush=True)
    index_b = build_file_index(pkg_b)
    print(f"  {sum(len(v) for v in index_b.values()):>6} files  ({len(index_b)} unique names)")

    sources_a = collect_sources(pkg_a)
    sources_b = collect_sources(pkg_b)
    total_files = len(sources_a) + len(sources_b)

    print(f"\nScanning {total_files} source files with {args.jobs} workers …")
    if args.dry_run:
        print("DRY-RUN — no files will be modified.\n")
    else:
        print()

    # --- Dispatch work ------------------------------------------------------
    # Each file in pkg_a looks in index_b, and vice-versa.
    work = (
        [(src, pkg_b.name, index_b, args.dry_run) for src in sources_a] +
        [(src, pkg_a.name, index_a, args.dry_run) for src in sources_b]
    )

    total_changes = 0
    total_unresolved = 0
    done = 0

    with ProcessPoolExecutor(max_workers=args.jobs) as pool:
        futures = {pool.submit(process_file, *w): w[0] for w in work}
        for future in as_completed(futures):
            changes, unresolved, logs = future.result()
            total_changes += changes
            total_unresolved += unresolved
            done += 1
            for msg in logs:
                # Clear progress line before printing a log entry
                print(f"\r{' ' * 50}\r{msg}")
            print(f"\r  {done}/{total_files} files …", end="", flush=True)

    # --- Summary ------------------------------------------------------------
    print(f"\r{' ' * 50}")
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