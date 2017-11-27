
def build_expat(cmake_lists):

    cmake_lists_section = open('expat.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["expat"] = build_expat
