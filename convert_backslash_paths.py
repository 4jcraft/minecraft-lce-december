#!/usr/bin/env python3
"""
Convert backslash C++ includes to forward slashes.
Replaces Windows-style paths in #include directives with Unix-style paths.
"""

import re
import sys
from pathlib import Path


def convert_includes(file_path):
    """
    Convert backslash includes to forward slashes in a C++ file.
    
    Args:
        file_path (str or Path): Path to the C++ file to process
        
    Returns:
        bool: True if file was modified, False otherwise
    """
    file_path = Path(file_path)
    
    if not file_path.exists():
        print(f"Error: File not found: {file_path}")
        return False
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
        return False
    
    original_content = content
    
    # Pattern to match #include directives with quoted paths
    # Matches: #include "path\to\file.h" or #include <path\to\file.h>
    pattern = r'(#include\s+[<"])([^>"<]+)([">\s])'
    
    def replace_backslashes(match):
        prefix = match.group(1)
        path = match.group(2)
        suffix = match.group(3)
        
        # Replace backslashes with forward slashes
        new_path = path.replace('\\', '/')
        
        return f"{prefix}{new_path}{suffix}"
    
    content = re.sub(pattern, replace_backslashes, content)
    
    # Check if any changes were made
    if content == original_content:
        print(f"No changes needed in {file_path}")
        return False
    
    # Write the modified content back
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Successfully updated {file_path}")
        return True
    except Exception as e:
        print(f"Error writing to file {file_path}: {e}")
        return False


def convert_directory(directory_path, extensions=None):
    """
    Convert backslash includes in all C++ files in a directory.
    
    Args:
        directory_path (str or Path): Path to the directory to process
        extensions (list): File extensions to process (default: ['.cpp', '.h', '.hpp', '.cc'])
    """
    if extensions is None:
        extensions = ['.cpp', '.h', '.hpp', '.cc', '.cxx', '.c++', '.hxx']
    
    directory_path = Path(directory_path)
    
    if not directory_path.is_dir():
        print(f"Error: Directory not found: {directory_path}")
        return
    
    modified_count = 0
    processed_count = 0
    
    for ext in extensions:
        for file_path in directory_path.rglob(f'*{ext}'):
            processed_count += 1
            if convert_includes(file_path):
                modified_count += 1
    
    print(f"\nProcessing complete: {processed_count} files checked, {modified_count} files modified")


def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  Single file:  python fix_includes.py <file_path>")
        print("  Directory:    python fix_includes.py --dir <directory_path>")
        print("\nExample:")
        print("  python fix_includes.py myfile.cpp")
        print("  python fix_includes.py --dir ./src")
        sys.exit(1)
    
    if sys.argv[1] == '--dir':
        if len(sys.argv) < 3:
            print("Error: Please provide a directory path")
            sys.exit(1)
        convert_directory(sys.argv[2])
    else:
        convert_includes(sys.argv[1])


if __name__ == '__main__':
    main()
