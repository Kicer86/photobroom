
ExternalProject_Add(jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 9234cbbc90d1f6c70dd5a90b4d533779e45c820c
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)
