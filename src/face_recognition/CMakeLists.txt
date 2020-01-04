
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(pybind11 REQUIRED)
find_package(dlib REQUIRED)

add_library(face_recognition
    face_recognition.cpp
    face_recognition.hpp
    dlib_wrapper/dlib_face_recognition_api.cpp
    dlib_wrapper/dlib_face_recognition_api.hpp
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
                                pybind11::embed
                                dlib
)

target_compile_definitions(face_recognition PRIVATE DQT_NO_KEYWORDS)

generate_export_header(face_recognition)
hideSymbols(face_recognition)

if(BUILD_SHARED_LIBS)
    install(TARGETS face_recognition RUNTIME DESTINATION ${PATH_LIBS}
                                     LIBRARY DESTINATION ${PATH_LIBS})
endif()

install(FILES
            scripts/_utils.py
            scripts/choose_best.py
            scripts/recognize_face.py
            scripts/system_test.py
        DESTINATION
            ${PATH_SCRIPTS}
)
