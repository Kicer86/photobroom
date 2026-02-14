import os
import pefile
import sys
from collections import defaultdict

checked_dlls = defaultdict(lambda: False)

def get_imported_dlls(pe):
    """Extract the list of imported DLLs from the PE file."""
    dlls = []
    if hasattr(pe, 'DIRECTORY_ENTRY_IMPORT'):
        for entry in pe.DIRECTORY_ENTRY_IMPORT:
            dlls.append(entry.dll.decode('utf-8'))
    return dlls

def check_dll_in_path(dll):
    """Check if a DLL exists in the system's PATH."""
    for directory in os.environ['PATH'].split(os.pathsep):
        dll_path = os.path.join(directory, dll)
        if os.path.exists(dll_path):
            return dll_path
    return None

def check_dll_in_local_dir(dll, exe_dir):
    """Check if a DLL exists in the same directory as the executable."""
    dll_path = os.path.join(exe_dir, dll)
    return dll_path if os.path.exists(dll_path) else None

def validate_dll(dll_path, exe_dir):
    """Validate a DLL and check its dependencies recursively."""
    if checked_dlls[dll_path]:
        return [], []

    checked_dlls[dll_path] = True
    missing_dlls = []
    invalid_dlls = []

    try:
        pe = pefile.PE(dll_path)
    except pefile.PEFormatError:
        invalid_dlls.append(dll_path)
        return missing_dlls, invalid_dlls

    dlls = get_imported_dlls(pe)

    for dll in dlls:
        # Skip DLLs starting with "api-ms-win-"
        if dll.lower().startswith("api-ms-win-"):
            continue

        dll_full_path = check_dll_in_local_dir(dll, exe_dir) or check_dll_in_path(dll)
        if not dll_full_path:
            missing_dlls.append(dll)
        else:
            rec_missing, rec_invalid = validate_dll(dll_full_path, os.path.dirname(dll_full_path))
            missing_dlls.extend(rec_missing)
            invalid_dlls.extend(rec_invalid)

    return missing_dlls, invalid_dlls

def validate_dependencies(exe_path):
    exe_dir = os.path.dirname(exe_path)
    missing_dlls, invalid_dlls = validate_dll(exe_path, exe_dir)
    return missing_dlls, invalid_dlls


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python dependencies_validator.py <path_to_exe>")
        sys.exit(1)

    exe_path = sys.argv[1]
    if not os.path.exists(exe_path):
        print(f"Error: The file '{exe_path}' does not exist.")
        sys.exit(1)

    missing_dlls, invalid_dlls = validate_dependencies(exe_path)

    exit_code = 0

    if missing_dlls:
        print("Missing DLLs:")
        for dll in missing_dlls:
            print(f" - {dll}")
        exit_code |= 2
    else:
        print("No missing DLLs found.")

    if invalid_dlls:
        print("Invalid DLLs:")
        for dll in invalid_dlls:
            print(f" - {dll}")
        exit_code |= 4
    else:
        print("No invalid DLLs found.")

    sys.exit(exit_code)
