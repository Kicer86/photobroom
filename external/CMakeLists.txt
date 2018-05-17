
include(ExternalProject)

ExternalProject_Add(dlib
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/dlib/tools/python
    BINARY_DIR ${PROJECT_BINARY_DIR}/dlib
    INSTALL_COMMAND ""
)

install(DIRECTORY face_recognition/face_recognition DESTINATION ${PATH_SCRIPTS})
install(DIRECTORY face_recognition_models/face_recognition_models DESTINATION ${PATH_SCRIPTS})

install(CODE "set(PATH_SCRIPTS \"${PATH_SCRIPTS}\")")
install(CODE "set(PROJECT_BINARY_DIR \"${PROJECT_BINARY_DIR}\")")
install(CODE "set(CMAKE_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
install(SCRIPT dlib.cmake)
