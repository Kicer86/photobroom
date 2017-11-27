
def build_zlib(cmake_lists):

    cmake_lists_section = open('zlib.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["zlib"] = build_zlib
