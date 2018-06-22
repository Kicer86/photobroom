
def build_dlib(cmake_lists):

    cmake_lists_section = open('dlib.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["dlib"] = build_dlib
