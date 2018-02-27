
include(ExternalProject)

ExternalProject_Add(dlib
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/dlib/tools/python
    BINARY_DIR ${PROJECT_BINARY_DIR}/dlib
    INSTALL_COMMAND ""
)

find_package(PythonLibs 3.0 REQUIRED)

add_library(face_recognition
    face_recognition.cpp
    face_recognition.hpp
)

target_include_directories(face_recognition PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(face_recognition PRIVATE ${PYTHON_LIBRARIES})
