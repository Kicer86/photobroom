
def build_exiv2(cmake_lists):

    cmake_lists_section = open('exiv2.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["exiv2"] = build_exiv2
