
ExternalProject_Add(exiv2
    DEPENDS zlib expat
    GIT_REPOSITORY https://github.com/Kicer86/exiv2.git
    GIT_TAG unique_ptr
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS}
)
