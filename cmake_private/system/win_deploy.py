
from sys import argv
import os


class Finder:
    def __init__(self):
        pass

    def _find_dll_s(self, name, search_path):
        for root, dirs, files in os.walk(search_path):
            for file in files:
                if file.endswith(".dll") and file.startswith(name):
                    return root + "/" + file

        return None

    def find_dll(self, name, search_paths):
        for searchPath in search_paths:
            path = self._find_dll_s(name, searchPath)

            if path:
                return path

        return None

    def collect_libraries(self, dependencies_dir):
        required_libs = \
        [
            "QtExt",                   # OpenLibrary
            "exiv2", "expat", "zlib",  # exiv2 + dependencies
        ]

        libraries = []
        for lib in required_libs:
            path = self.find_dll(lib, [dependencies_dir])

            if path:
                libraries.append(path)
            else:
                print("Could not find dll for " + lib)
                exit(1)

        return libraries

    def collect_qt_files(self, binaries, output):
        for binary in binaries:
            pass


if len(argv) != 2:
    print("Invalid arguments")
    exit(1)

dependencies_dir = argv[1]

finder = Finder()

libs = finder.collect_libraries(dependencies_dir)
