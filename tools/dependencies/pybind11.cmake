
ExternalProject_Add(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.2.4
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS} -DPYBIND11_TEST=OFF
)
