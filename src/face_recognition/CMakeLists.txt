
find_package(Qt5Core REQUIRED)

add_library(face_recognition
    face_recognition.cpp
    face_recognition.hpp
)

target_include_directories(face_recognition
                                PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                PRIVATE
                                    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(face_recognition
                            PUBLIC
                                Qt5::Core
                            PRIVATE
                                system
                                core
)

if(BUILD_SHARED_LIBS)
    install(TARGETS face_recognition RUNTIME DESTINATION ${PATH_LIBS}
                                     LIBRARY DESTINATION ${PATH_LIBS})
endif()

install(FILES
            scripts/find_faces.py
        DESTINATION
            ${PATH_SCRIPTS}
)
