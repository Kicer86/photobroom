
ExternalProject_Add(zlib
    URL http://zlib.net/zlib-1.2.11.tar.xz
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS}
)
