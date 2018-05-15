
ExternalProject_Add(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS}
)
