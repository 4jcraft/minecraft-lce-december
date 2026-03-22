#!/usr/bin/env python3
"""
fix_std_namespace.py

Replaces unqualified C++11 standard library names (brought in via
`using namespace std;`) with their fully-qualified `std::` counterparts.

Strategy
--------
* Pure text/regex — no libclang, no preprocessing required.
* Skips names that are already inside a `std::` qualification.
* Skips occurrences inside string literals and block/line comments.
* Handles `using namespace std;` that is file-scoped OR nested inside a
  function/block (best-effort: treats every file as potentially affected).
* Parallel file processing via `concurrent.futures.ProcessPoolExecutor`.

Usage
-----
    # Dry-run a single file
    python fix_std_namespace.py file.cpp

    # In-place fix an entire tree with 8 workers
    python fix_std_namespace.py --inplace -j 8 src/

    # Show a unified diff without touching anything
    python fix_std_namespace.py --diff src/

    # Only process files that actually contain `using namespace std`
    python fix_std_namespace.py --only-if-using --inplace src/
"""

from __future__ import annotations

import argparse
import difflib
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path
from typing import List, Optional, Tuple

# ---------------------------------------------------------------------------
# The symbol table
# ---------------------------------------------------------------------------
# Maps every unqualified C++11 std name → its std:: version.
# Organised by header for readability; order does not matter.

STD_SYMBOLS: dict[str, str] = {}

_RAW_SYMBOLS: list[tuple[str, list[str]]] = [
    # <algorithm>
    ("std::{}", [
        "accumulate", "adjacent_difference", "adjacent_find", "all_of",
        "any_of", "binary_search", "copy_backward", "copy_if",
        "copy_n", "count_if", "equal", "equal_range",
        "fill", "fill_n", "find", "find_end", "find_first_of", "find_if",
        "find_if_not", "for_each", "generate", "generate_n",
        "includes", "inplace_merge", "is_heap", "is_heap_until",
        "is_partitioned", "is_permutation", "is_sorted", "is_sorted_until",
        "iter_swap", "lexicographical_compare", "lower_bound",
        "make_heap", "max_element", "merge", "min_element",
        "minmax", "minmax_element", "mismatch", "move_backward",
        "next_permutation", "none_of", "nth_element", "partial_sort",
        "partial_sort_copy", "partial_sum", "partition", "partition_copy",
        "partition_point", "pop_heap", "prev_permutation", "push_heap",
        "random_shuffle", "remove_copy", "remove_copy_if",
        "remove_if", "replace_copy", "replace_copy_if",
        "replace_if", "reverse_copy", "rotate_copy",
        "search", "search_n", "set_difference", "set_intersection",
        "set_symmetric_difference", "set_union", "shuffle",
        "sort_heap", "stable_partition", "stable_sort",
        "swap_ranges", "transform", "unique_copy", "upper_bound",
    ]),
    # <atomic>
    ("std::{}", [
        "atomic_bool", "atomic_char", "atomic_int",
        "atomic_uint", "atomic_long", "atomic_ulong", "atomic_llong",
        "atomic_ullong", "atomic_flag",
        "atomic_thread_fence", "atomic_signal_fence", "atomic_store",
        "atomic_load", "atomic_exchange", "atomic_compare_exchange_weak",
        "atomic_compare_exchange_strong",
    ]),
    # <bitset>
    ("std::{}", ["bitset"]),
    # <cassert / cstdlib / etc. macros are not in std::, skip>
    # <condition_variable>
    ("std::{}", ["condition_variable", "condition_variable_any", "notify_all_at_thread_exit"]),
    # <deque>
    ("std::{}", ["deque"]),
    # <exception>
    ("std::{}", [
        "exception", "bad_exception", "terminate_handler",
        "unexpected_handler", "terminate", "unexpected",
        "current_exception", "rethrow_exception", "make_exception_ptr",
        "exception_ptr", "nested_exception", "throw_with_nested",
        "rethrow_if_nested",
    ]),
    # <forward_list>
    ("std::{}", ["forward_list"]),
    # <functional>
    ("std::{}", [
        "function", "mem_fn", "cref",
        "reference_wrapper", "hash",
        "greater_equal", "less_equal",
        "logical_and", "logical_or", "logical_not",
        "bit_and", "bit_or", "bit_xor", "bit_not",
        "unary_negate", "binary_negate", "not1", "not2",
        "is_bind_expression", "is_placeholder", "placeholders",
    ]),
    # <ios> / <iostream> / <istream> / <ostream> / <sstream> / <fstream>
    ("std::{}", [
        "ios", "ios_base", "basic_ios",
        "istream", "ostream", "iostream",
        "ifstream", "ofstream", "fstream",
        "istringstream", "ostringstream", "stringstream",
        "cin", "cout", "cerr", "clog",
        "wcin", "wcout", "wcerr", "wclog",
        "endl",
        "boolalpha", "noboolalpha", "showbase", "noshowbase",
        "showpoint", "noshowpoint", "showpos", "noshowpos",
        "skipws", "noskipws", "uppercase", "nouppercase",
        "unitbuf", "nounitbuf",
        "hexfloat", "defaultfloat", "getline",
        "streamsize", "streampos", "streamoff",
        "char_traits", "basic_streambuf", "streambuf",
        "basic_stringbuf", "basic_filebuf",
    ]),
    # <limits>
    ("std::{}", ["numeric_limits", "float_round_style", "float_denorm_style"]),
    # <map>
    ("std::{}", ["multimap"]),
    # <memory>
    ("std::{}", [
        "unique_ptr", "shared_ptr", "weak_ptr", "auto_ptr",
        "make_shared", "make_unique",
        "allocator", "allocator_traits",
        "raw_storage_iterator", "default_delete",
        "enable_shared_from_this", "owner_less",
        "pointer_traits", "addressof",
        "uninitialized_copy", "uninitialized_copy_n",
        "uninitialized_fill", "uninitialized_fill_n",
    ]),
    # <new>
    ("std::{}", [
        "bad_alloc", "bad_array_new_length", "nothrow_t", "nothrow",
        "new_handler", "set_new_handler", "get_new_handler",
        "align_val_t",
    ]),
    # <numeric>
    ("std::{}", [
        "iota", "accumulate", "inner_product", "partial_sum",
        "adjacent_difference",
    ]),
    # <queue>
    ("std::{}", ["queue", "priority_queue"]),
    # <random>
    ("std::{}", [
        "default_random_engine", "mt19937", "mt19937_64",
        "minstd_rand", "minstd_rand0",
        "ranlux24_base", "ranlux48_base", "ranlux24", "ranlux48",
        "knuth_b",
        "random_device",
        "uniform_int_distribution", "uniform_real_distribution",
        "bernoulli_distribution", "binomial_distribution",
        "negative_binomial_distribution", "geometric_distribution",
        "poisson_distribution", "exponential_distribution",
        "gamma_distribution", "weibull_distribution",
        "extreme_value_distribution", "normal_distribution",
        "lognormal_distribution", "chi_squared_distribution",
        "cauchy_distribution", "fisher_f_distribution",
        "student_t_distribution",
        "discrete_distribution", "piecewise_constant_distribution",
        "piecewise_linear_distribution",
        "seed_seq", "generate_canonical",
    ]),
    # <regex>
    ("std::{}", [
        "regex", "wregex", "basic_regex",
        "match_results", "cmatch", "wcmatch", "smatch", "wsmatch",
        "sub_match", "csub_match", "wcsub_match", "ssub_match", "wssub_match",
        "regex_iterator", "cregex_iterator", "wcregex_iterator",
        "sregex_iterator", "wsregex_iterator",
        "regex_token_iterator",
        "regex_match", "regex_search", "regex_replace",
        "regex_error", "regex_constants",
    ]),
    # <set>
    ("std::{}", ["multiset"]),
    # <stdexcept>
    ("std::{}", [
        "logic_error", "invalid_argument", "domain_error",
        "length_error", "out_of_range",
        "runtime_error", "range_error", "overflow_error", "underflow_error",
    ]),
    # <string>
    ("std::{}", [
        "string", "wstring", "u16string", "u32string",
        "basic_string", "char_traits",
        "stoi", "stol", "stoll", "stoul", "stoull",
        "stof", "stod", "stold",
        "to_string", "to_wstring",
    ]),
    ("std::this_thread::{}", [
        "get_id", "yield", "sleep_for", "sleep_until",
    ]),
    # <tuple>
    ("std::{}", [
        "tuple", "make_tuple", "forward_as_tuple", "tuple_cat",
        "tuple_size", "tuple_element",
    ]),
    # <type_traits>
    ("std::{}", [
        "integral_constant", "true_type", "false_type",
        "is_void", "is_null_pointer", "is_integral", "is_floating_point",
        "is_array", "is_enum", "is_union", "is_class", "is_function",
        "is_pointer", "is_lvalue_reference", "is_rvalue_reference",
        "is_member_object_pointer", "is_member_function_pointer",
        "is_fundamental", "is_arithmetic", "is_scalar",
        "is_object", "is_compound", "is_reference",
        "is_member_pointer",
        "is_const", "is_volatile",
        "is_trivial", "is_trivially_copyable", "is_standard_layout",
        "is_pod", "is_literal_type", "is_empty", "is_polymorphic",
        "is_abstract", "is_final",
        "is_signed", "is_unsigned",
        "is_constructible", "is_trivially_constructible",
        "is_nothrow_constructible",
        "is_default_constructible", "is_trivially_default_constructible",
        "is_nothrow_default_constructible",
        "is_copy_constructible", "is_trivially_copy_constructible",
        "is_nothrow_copy_constructible",
        "is_move_constructible", "is_trivially_move_constructible",
        "is_nothrow_move_constructible",
        "is_assignable", "is_trivially_assignable", "is_nothrow_assignable",
        "is_copy_assignable", "is_trivially_copy_assignable",
        "is_nothrow_copy_assignable",
        "is_move_assignable", "is_trivially_move_assignable",
        "is_nothrow_move_assignable",
        "is_destructible", "is_trivially_destructible",
        "is_nothrow_destructible", "has_virtual_destructor",
        "alignment_of", "rank", "extent",
        "is_same", "is_base_of", "is_convertible",
        "remove_const", "remove_volatile", "remove_cv",
        "add_const", "add_volatile", "add_cv",
        "remove_reference", "add_lvalue_reference", "add_rvalue_reference",
        "remove_pointer", "add_pointer",
        "make_signed", "make_unsigned",
        "remove_extent", "remove_all_extents",
        "aligned_storage", "aligned_union", "decay",
        "enable_if", "conditional", "common_type",
        "underlying_type", "result_of",
        "void_t", "conjunction", "disjunction", "negation",
        "invoke_result",
    ]),
    # <typeindex>
    ("std::{}", ["type_index"]),
    # <typeinfo>
    ("std::{}", ["type_info", "bad_cast", "bad_typeid"]),
    # <unordered_map>
    ("std::{}", ["unordered_map", "unordered_multimap"]),
    # <unordered_set>
    ("std::{}", ["unordered_set", "unordered_multiset"]),
    # <utility>
    ("std::{}", [
        "pair", "make_pair", "forward",
        "move_if_noexcept", "declval",
        "piecewise_construct", "piecewise_construct_t",
        "integer_sequence", "index_sequence",
        "make_integer_sequence", "make_index_sequence",
        "index_sequence_for",
    ]),
    # <valarray>
    ("std::{}", ["valarray", "slice", "gslice", "mask_array", "indirect_array"]),
    # <variant> (C++17 but often requested alongside C++11 cleanups)
    ("std::{}", [
        "monostate", "bad_variant_access",
        "get_if", "holds_alternative", "visit",
        "variant_size", "variant_alternative",
    ]),
    # <vector>
    ("std::{}", ["vector"]),
]

for _qualified_fmt, _names in _RAW_SYMBOLS:
    for _name in _names:
        _qualified = _qualified_fmt.format(_name)
        # Only add if not already mapped to something longer/specific
        if _name not in STD_SYMBOLS:
            STD_SYMBOLS[_name] = _qualified

# ---------------------------------------------------------------------------
# Tokeniser helpers — strip comments and strings before matching
# ---------------------------------------------------------------------------

# Matches:
#   //...  line comments
#   /*...*/  block comments
#   "..."  string literals (with escape handling)
#   '...'  char literals
#   R"delim(...)delim"  raw string literals
_TOKEN_RE = re.compile(
    r'//[^\n]*'                         # line comment
    r'|/\*.*?\*/'                       # block comment
    r'|R"([^()\s]{0,16})\('            # raw string open  — group 1 = delimiter
    r'|"(?:[^"\\]|\\.)*"'              # regular string literal
    r"|'(?:[^'\\]|\\.)*'"              # char literal
    ,
    re.DOTALL,
)

_RAW_STRING_CLOSE_CACHE: dict[str, re.Pattern] = {}


def _tokenise(source: str) -> list[tuple[str, int, int, bool]]:
    """
    Return a list of (text, start, end, is_code) spans covering the whole file.
    is_code=False means the span is a comment or string literal — don't touch it.
    """
    spans: list[tuple[str, int, int, bool]] = []
    pos = 0
    for m in _TOKEN_RE.finditer(source):
        s, e = m.start(), m.end()
        if s > pos:
            spans.append((source[pos:s], pos, s, True))
        text = source[s:e]

        # Handle raw string: the regex only matched the opening part
        if m.lastindex == 1:  # raw string group matched
            delim = m.group(1)
            close = f"){delim}\""
            close_pos = source.find(close, e)
            if close_pos == -1:
                # Malformed — treat rest of file as non-code
                spans.append((source[s:], s, len(source), False))
                pos = len(source)
                break
            e = close_pos + len(close)
            text = source[s:e]

        spans.append((text, s, e, False))
        pos = e

    if pos < len(source):
        spans.append((source[pos:], pos, len(source), True))

    return spans


# ---------------------------------------------------------------------------
# Core replacement logic
# ---------------------------------------------------------------------------

# We build one large regex that matches any of the unqualified names,
# but NOT when already preceded by `::` or `std::` etc.
# Pattern: a negative lookbehind for `::` then a word boundary, then the
# name, then a word boundary, then a negative lookahead for `::` (so we
# don't double-qualify things like `std::string::npos`).

# Build sorted-longest-first to avoid partial matches
_SORTED_NAMES = sorted(STD_SYMBOLS.keys(), key=len, reverse=True)

_REPLACE_RE = re.compile(
    r'(?<![:\w])(?<!->)(?<!\.)(?<!\#)\b(' + '|'.join(re.escape(n) for n in _SORTED_NAMES) + r')\b(?!::)',
)


def _qualify_code_span(code: str) -> str:
    """Apply qualified replacements to a single code span, skipping preprocessor lines."""
    lines = code.split("\n")
    result = []
    for line in lines:
        # Skip preprocessor directives (#include, #define, #if, etc.)
        if re.match(r"^\s*#", line):
            result.append(line)
        else:
            def _repl(m: re.Match) -> str:
                name = m.group(1)
                return STD_SYMBOLS[name]
            result.append(_REPLACE_RE.sub(_repl, line))
    return "\n".join(result)


def fix_source(source: str, only_if_using: bool = False) -> str:
    """
    Return the fixed version of *source*.
    If only_if_using is True, skip files that have no `using namespace std`.
    """
    if only_if_using:
        # Quick pre-check — look outside comments/strings
        spans = _tokenise(source)
        has_using = any(
            re.search(r'\busing\s+namespace\s+std\s*;', span)
            for span, _, _, is_code in spans
            if is_code
        )
        if not has_using:
            return source

    spans = _tokenise(source)
    parts: list[str] = []
    for text, _start, _end, is_code in spans:
        if is_code:
            parts.append(_qualify_code_span(text))
        else:
            parts.append(text)
    return "".join(parts)


# ---------------------------------------------------------------------------
# File helpers
# ---------------------------------------------------------------------------

_CPP_EXTENSIONS = {
    ".c", ".cc", ".cpp", ".cxx", ".c++",
    ".h", ".hh", ".hpp", ".hxx", ".h++",
    ".tcc", ".ipp", ".inl",
}


def _should_process(path: Path) -> bool:
    return path.suffix.lower() in _CPP_EXTENSIONS


def _collect_files(paths: list[Path], recursive: bool) -> list[Path]:
    result: list[Path] = []
    for p in paths:
        if p.is_file():
            if _should_process(p):
                result.append(p)
        elif p.is_dir():
            if recursive:
                for child in p.rglob("*"):
                    if child.is_file() and _should_process(child):
                        result.append(child)
            else:
                for child in p.iterdir():
                    if child.is_file() and _should_process(child):
                        result.append(child)
    return result


# ---------------------------------------------------------------------------
# Worker (must be module-level for multiprocessing pickling)
# ---------------------------------------------------------------------------

def _process_file(
    path_str: str,
    inplace: bool,
    show_diff: bool,
    only_if_using: bool,
) -> tuple[str, Optional[str], Optional[str]]:
    """
    Returns (path_str, diff_or_None, error_or_None).
    """
    path = Path(path_str)
    try:
        original = path.read_text(encoding="utf-8", errors="replace")
    except OSError as exc:
        return path_str, None, str(exc)

    fixed = fix_source(original, only_if_using=only_if_using)

    if fixed == original:
        return path_str, None, None  # No changes

    diff: Optional[str] = None
    if show_diff:
        diff = "".join(
            difflib.unified_diff(
                original.splitlines(keepends=True),
                fixed.splitlines(keepends=True),
                fromfile=f"a/{path}",
                tofile=f"b/{path}",
            )
        )

    if inplace:
        try:
            path.write_text(fixed, encoding="utf-8")
        except OSError as exc:
            return path_str, diff, str(exc)

    return path_str, diff, None


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "paths",
        nargs="+",
        type=Path,
        metavar="PATH",
        help="Files or directories to process.",
    )
    p.add_argument(
        "--inplace", "-i",
        action="store_true",
        help="Write changes back to files.",
    )
    p.add_argument(
        "--diff", "-d",
        action="store_true",
        help="Print a unified diff for each changed file.",
    )
    p.add_argument(
        "--no-recursive", "-R",
        dest="recursive",
        action="store_false",
        default=True,
        help="Do not recurse into subdirectories.",
    )
    p.add_argument(
        "--only-if-using",
        action="store_true",
        help="Only process files that contain `using namespace std;`.",
    )
    p.add_argument(
        "-j", "--jobs",
        type=int,
        default=os.cpu_count() or 1,
        metavar="N",
        help="Number of parallel worker processes (default: cpu count).",
    )
    p.add_argument(
        "--list-symbols",
        action="store_true",
        help="Print all known symbol mappings and exit.",
    )
    return p


def main(argv: Optional[List[str]] = None) -> int:
    parser = _build_parser()
    args = parser.parse_args(argv)

    if args.list_symbols:
        for name, qualified in sorted(STD_SYMBOLS.items()):
            print(f"{name:50s} -> {qualified}")
        return 0

    files = _collect_files(args.paths, recursive=args.recursive)
    if not files:
        print("No C/C++ files found.", file=sys.stderr)
        return 1

    changed = 0
    errors = 0

    jobs = max(1, args.jobs)
    with ProcessPoolExecutor(max_workers=jobs) as exe:
        futures = {
            exe.submit(
                _process_file,
                str(f),
                args.inplace,
                args.diff,
                args.only_if_using,
            ): f
            for f in files
        }
        for fut in as_completed(futures):
            path_str, diff, error = fut.result()
            if error:
                print(f"ERROR {path_str}: {error}", file=sys.stderr)
                errors += 1
            elif diff is not None or (not args.diff and diff is None and not args.inplace):
                # diff is not None ↔ file changed
                if diff is not None:
                    changed += 1
                    if args.diff:
                        sys.stdout.write(diff)
                    elif not args.inplace:
                        print(f"WOULD CHANGE: {path_str}")
                    else:
                        print(f"FIXED: {path_str}")

    print(
        f"\nDone. {len(files)} file(s) scanned, "
        f"{changed} changed, {errors} error(s).",
        file=sys.stderr,
    )
    return 0 if errors == 0 else 2


if __name__ == "__main__":
    sys.exit(main())