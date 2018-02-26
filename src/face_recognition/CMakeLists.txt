
include(ExternalProject)

ExternalProject_Add(dlib
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/dlib/tools/python
    BINARY_DIR ${PROJECT_BINARY_DIR}/dlib
    INSTALL_COMMAND ""
)
