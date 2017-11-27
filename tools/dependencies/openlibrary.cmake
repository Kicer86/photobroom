
ExternalProject_Add(openlibrary
    GIT_REPOSITORY https://github.com/Kicer86/openlibrary.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS ${COMMON_OPTIONS}
)
