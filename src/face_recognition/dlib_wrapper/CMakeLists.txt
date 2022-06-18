
find_package(Qt6 REQUIRED COMPONENTS Core Gui)
find_package(dlib REQUIRED)

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
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
                                    ${DLIB_INCLUDE_DIRS}
)

target_link_libraries(dlib_wrapper
                            PUBLIC
                                Qt::Core
                                Qt::Gui
                                dlib::dlib
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
