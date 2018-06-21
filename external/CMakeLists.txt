
include(ExternalProject)

ExternalProject_Add(dlib
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/dlib/tools/python
    BINARY_DIR ${PROJECT_BINARY_DIR}/dlib
    INSTALL_COMMAND ""
)

install(DIRECTORY face_recognition/face_recognition DESTINATION ${PATH_SCRIPTS})
install(DIRECTORY face_recognition_models/face_recognition_models DESTINATION ${PATH_SCRIPTS})

# TODO: not very nice (EXCLUDE patterns), but I could not find anything better
install(DIRECTORY ${PROJECT_BINARY_DIR}/dlib/
        DESTINATION ${PATH_SCRIPTS}
        FILES_MATCHING
        PATTERN "dlib.cpython*"
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "dlib_build" EXCLUDE
        PATTERN "pybind11_build" EXCLUDE
)
