
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(dlib_target dlib)

if(TARGET CONAN_PKG::dlib)
    set(dlib_target CONAN_PKG::dlib)
else()
    # using conan here would be nice but due to https://github.com/omaralvarez/conan_dlib/issues/5 it cannot be used (always)
    # So instead dlib is being downloaded and build
    if(MSVC)
        # based on http://dlib.net/examples/CMakeLists.txt.html
        cmake_minimum_required(VERSION 3.14)
        include(FetchContent)
        FetchContent_Declare(dlib
            GIT_REPOSITORY https://github.com/davisking/dlib.git
            GIT_TAG        v19.19
        )
        FetchContent_MakeAvailable(dlib)
        set(dlib_target dlib::dlib)
    else()
        find_package(Threads REQUIRED)
        find_package(dlib REQUIRED)
        set(dlib_target dlib::dlib)

        get_cmake_property(_variableNames VARIABLES)
        list (SORT _variableNames)
        foreach (_variableName ${_variableNames})
            message(STATUS "${_variableName}=${${_variableName}}")
        endforeach()

    endif()
endif()

add_library(dlib_wrapper
    cnn_face_detector.cpp
    cnn_face_detector.hpp
    dlib_face_recognition_api.cpp
    dlib_face_recognition_api.hpp
    face_recognition.cpp
    face_recognition.hpp
    helpers.cpp
    helpers.hpp
)

target_include_directories(dlib_wrapper
                                PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
                                PRIVATE
                                    ${DLIB_INCLUDE_DIRS}
)

target_link_libraries(dlib_wrapper
                            PUBLIC
                                Qt5::Core
                                Qt5::Gui
                                ${dlib_target}
                            PRIVATE
                                core
                                system
)

generate_export_header(dlib_wrapper)
hideSymbols(dlib_wrapper)

if(BUILD_SHARED_LIBS)
    install(TARGETS dlib_wrapper RUNTIME DESTINATION ${PATH_LIBS}
                                 LIBRARY DESTINATION ${PATH_LIBS})
endif()

# face recognition models
file(GLOB models face_recognition_models/face_recognition_models/models/*.dat)

install(FILES
            ${models}
        DESTINATION
            ${PATH_DATA}/face_recognition_models
)

