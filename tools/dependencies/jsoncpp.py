

def build_jsoncpp(cmake_lists):

    cmake_lists.write(
        "ExternalProject_Add(jsoncpp                                       \n"
        "GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git \n"
        "GIT_TAG 9234cbbc90d1f6c70dd5a90b4d533779e45c820c                  \n"
        "PREFIX ${PROJECT_BINARY_DIR}                                      \n"
        "CMAKE_ARGS ${COMMON_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON \n"
        ")                                                                 \n"
        )

    return 0


available_packages["jsoncpp"] = build_jsoncpp
