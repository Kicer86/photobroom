
from sys import argv
import os


def _findDll(name, searchPath):
    for root, dirs, files in os.walk(searchPath):
        for file in files:
            if file.endswith(".dll") and file.startswith(name):
                return root + "/" + file

    return None


def findDll(name, searchPaths):
    for searchPath in searchPaths:
        path = _findDll(name, searchPath)

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
    path = findDll(lib, [dependencies_dir])

    if path:
        print("lib = ", path)
    else:
        print("lib = :(")
