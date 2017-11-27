
def build_jsoncpp(cmake_lists):

    cmake_lists_section = open('jsoncpp.cmake').read()
    cmake_lists.write(cmake_lists_section)

    return 0


available_packages["jsoncpp"] = build_jsoncpp
