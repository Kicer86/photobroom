
ExternalProject_Add(expat
    GIT_REPOSITORY https://github.com/libexpat/libexpat
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS} -DBUILD_tests=FALSE -DCMAKE_DEBUG_POSTFIX=
    SOURCE_SUBDIR expat
)
