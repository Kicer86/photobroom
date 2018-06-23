
from sys import argv
import os


def _find_dll(name, search_path):
    for root, dirs, files in os.walk(search_path):
        for file in files:
            if file.endswith(".dll") and file.startswith(name):
                return root + "/" + file

    return None


def find_dll(name, search_paths):
    for searchPath in search_paths:
        path = _find_dll(name, searchPath)

        if path:
            return path

    return None


# collect necessary binaries
required_libs = \
    [
        "QtExt",                   # from OpenLibrary
        "exiv2", "expat", "zlib",  # exiv2 + dependencies
    ]

if len(argv) != 2:
    print("Invalid arguments")
    exit(1)

dependencies_dir = argv[1]

for lib in required_libs:
    path = find_dll(lib, [dependencies_dir])

    if path:
        print("lib = ", path)
    else:
        print("lib = :(")
