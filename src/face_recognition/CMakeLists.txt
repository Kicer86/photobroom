
add_subdirectory(learning_tests)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(dlib_target dlib)

if(TARGET CONAN_PKG::dlib)
    set(dlib_target CONAN_PKG::dlib)
else()
    find_package(dlib QUIET)
    # using conan here would be nice but due to https://github.com/omaralvarez/conan_dlib/issues/5 it cannot be used (always)
    # So instead dlib is being downloaded and build
    if(dlib_FOUND)
        set(dlib_target dlib)
    else()
        # based on http://dlib.net/examples/CMakeLists.txt.html
        cmake_minimum_required(VERSION 3.14)
        include(FetchContent)
        FetchContent_Declare(dlib
            GIT_REPOSITORY https://github.com/davisking/dlib.git
            GIT_TAG        v19.19
        )
        FetchContent_MakeAvailable(dlib)
        set(dlib_target dlib::dlib)
    endif()
endif()

add_library(face_recognition
    face_recognition.cpp
    face_recognition.hpp
    dlib_wrapper/cnn_face_detector.cpp
    dlib_wrapper/cnn_face_detector.hpp
    dlib_wrapper/dlib_face_recognition_api.cpp
    dlib_wrapper/dlib_face_recognition_api.hpp
    dlib_wrapper/face_recognition.cpp
    dlib_wrapper/face_recognition.hpp
    dlib_wrapper/helpers.cpp
    dlib_wrapper/helpers.hpp
)

target_include_directories(face_recognition
                                PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                PRIVATE
                                    ${CMAKE_SOURCE_DIR}/src
                                    ${DLIB_INCLUDE_DIRS}
)

target_link_libraries(face_recognition
                            PUBLIC
                                Qt5::Core
                                Qt5::Gui
                            PRIVATE
                                core
                                database
                                system
                                ${dlib_target}
)

target_compile_definitions(face_recognition PRIVATE DQT_NO_KEYWORDS)

generate_export_header(face_recognition)
hideSymbols(face_recognition)

if(BUILD_SHARED_LIBS)
    install(TARGETS face_recognition RUNTIME DESTINATION ${PATH_LIBS}
                                     LIBRARY DESTINATION ${PATH_LIBS})
endif()

# face recognition models
file(GLOB models face_recognition_models/face_recognition_models/models/*.dat)

install(FILES
            ${models}
        DESTINATION
            ${PATH_DATA}/face_recognition_models
)
