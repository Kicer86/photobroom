
def build_openlibrary(cmake_lists):

    cmake_lists_section = open('openlibrary.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["openlibrary"] = build_openlibrary
