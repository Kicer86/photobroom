
add_subdirectory(learning_tests)
add_subdirectory(dlib_wrapper)

find_package(Qt5 REQUIRED COMPONENTS Core Gui)

add_library(face_recognition
    face_recognition.cpp
    face_recognition.hpp
)

target_include_directories(face_recognition
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(face_recognition
                            PUBLIC
                                Qt::Core
                                Qt::Gui
                            PRIVATE
                                core
                                database
                                dlib_wrapper
                                system
)

generate_export_header(face_recognition)
hideSymbols(face_recognition)

if(BUILD_SHARED_LIBS)
    install(TARGETS face_recognition RUNTIME DESTINATION ${PATH_LIBS}
                                     LIBRARY DESTINATION ${PATH_LIBS})
endif()
