
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(dlib_target dlib)

if(TARGET CONAN_PKG::dlib)
    set(dlib_target CONAN_PKG::dlib)
else()
    find_package(dlib REQUIRED)
    set(dlib_target dlib)
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
                            PRIVATE
                                ${dlib_target}
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

