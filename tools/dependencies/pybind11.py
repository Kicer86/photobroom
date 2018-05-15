
def build_pybind11(cmake_lists):

    cmake_lists_section = open('pybind11.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["pybind11"] = build_pybind11
