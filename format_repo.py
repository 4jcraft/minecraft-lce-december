#!/usr/bin/env python3
"""
Fast parallel clang-format runner with directory exclusion support.
Processes all .c, .cpp, .h, .cc, .cxx, .hpp files in a repository,
excluding specified directory globs.
"""

import argparse
import os
import sys
import subprocess
import fnmatch
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Set

def should_exclude(file_path: Path, exclude_patterns: List[str]) -> bool:
    """Check if a file path matches any exclusion patterns."""
    path_str = str(file_path)
    for pattern in exclude_patterns:
        if fnmatch.fnmatch(path_str, pattern):
            return True
    return False


def find_source_files(repo_root: Path, exclude_patterns: List[str]) -> List[Path]:
    """
    Efficiently find all C/C++ source files, excluding specified patterns.
    Uses glob patterns for better performance than recursive walking.
    """
    source_extensions = {'.c', '.cpp', '.h', '.cc', '.cxx', '.hpp'}
    source_files = []
    
    # Use rglob with pathlib for better performance
    for ext in source_extensions:
        for file_path in repo_root.rglob(f'*{ext}'):
            if not should_exclude(file_path, exclude_patterns):
                source_files.append(file_path)
    
    return source_files


def format_file(file_path: Path, in_place: bool = True, dry_run: bool = False) -> tuple:
    """
    Run clang-format on a single file.
    Returns: (file_path, success, error_message)
    """
    try:
        cmd = ['clang-format']
        
        if in_place:
            cmd.append('-i')
        
        cmd.append(str(file_path))
        
        if dry_run:
            # For dry run, just check if file would be reformatted
            result = subprocess.run(
                cmd + ['--output-replacements-xml'],
                capture_output=True,
                timeout=10
            )
            has_changes = b'<replacement' in result.stdout
            return (file_path, True, f"Would reformat: {has_changes}")
        else:
            # Run clang-format
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=10
            )
            
            if result.returncode != 0:
                return (file_path, False, result.stderr)
            return (file_path, True, None)
    
    except subprocess.TimeoutExpired:
        return (file_path, False, "Timeout")
    except FileNotFoundError:
        return (file_path, False, "clang-format not found in PATH")
    except Exception as e:
        return (file_path, False, str(e))


def main():
    parser = argparse.ArgumentParser(
        description='Fast parallel clang-format runner for C/C++ repositories',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Examples:
  # Format entire repo
  python format_repo.py /path/to/repo
  
  # Format with exclusions
  python format_repo.py /path/to/repo --exclude "*/build/*" "*/third_party/*"
  
  # Dry run (check without modifying)
  python format_repo.py /path/to/repo --dry-run
  
  # Use 4 threads
  python format_repo.py /path/to/repo --jobs 4
        '''
    )
    
    parser.add_argument(
        'repo_root',
        help='Root directory of the repository'
    )
    parser.add_argument(
        '--exclude',
        nargs='+',
        default=[],
        help='Directory glob patterns to exclude (e.g., "*/build/*" "*/third_party/*")'
    )
    parser.add_argument(
        '--jobs',
        type=int,
        default=None,
        help='Number of parallel threads (default: CPU count)'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Check formatting without modifying files'
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Print verbose output'
    )
    
    args = parser.parse_args()
    
    repo_root = Path(args.repo_root)
    if not repo_root.exists():
        print(f"Error: Repository root '{repo_root}' does not exist", file=sys.stderr)
        sys.exit(1)
    
    # Normalize exclude patterns for absolute paths
    exclude_patterns = []
    for pattern in args.exclude:
        exclude_patterns.append(str(repo_root / pattern))
    
    print(f"Scanning repository: {repo_root}")
    
    # Find all source files
    source_files = find_source_files(repo_root, exclude_patterns)
    
    if not source_files:
        print("No C/C++ source files found")
        return 0
    
    print(f"Found {len(source_files)} files to format")
    if args.verbose:
        for f in sorted(source_files):
            print(f"  {f}")
    
    # Format files in parallel
    max_workers = args.jobs
    formatted = 0
    failed = 0
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {
            executor.submit(format_file, f, in_place=not args.dry_run, dry_run=args.dry_run): f
            for f in source_files
        }
        
        for future in as_completed(futures):
            file_path, success, error = future.result()
            
            if success:
                formatted += 1
                if args.verbose or args.dry_run:
                    status = error if error else "Formatted"
                    print(f"  ✓ {file_path.relative_to(repo_root)}: {status}")
            else:
                failed += 1
                print(f"  ✗ {file_path.relative_to(repo_root)}: {error}", file=sys.stderr)
    
    # Summary
    print(f"\nProcessed: {formatted} successful, {failed} failed out of {len(source_files)} files")
    
    if args.dry_run:
        print("(Dry run - no files were modified)")
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())