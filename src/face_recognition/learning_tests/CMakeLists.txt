

find_program(WGET wget)

if(WGET)

    find_package(Threads REQUIRED)
    find_package(GTest REQUIRED)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)

    # download two fake faces from https://thispersondoesnotexist.com
    add_custom_command(OUTPUT face1.jpg face2.jpg
                    COMMAND ${WGET}
                                --header="Accept: text/html"
                                --user-agent="Mozilla/5.0 Gecko/20100101 Firefox/21.0"
                                -nv
                                -O ${CMAKE_CURRENT_BINARY_DIR}/face1.jpg
                                https://thispersondoesnotexist.com/image

                    COMMAND ${CMAKE_COMMAND} -E sleep 3

                    COMMAND ${WGET}
                                --header="Accept: text/html"
                                --user-agent="Mozilla/5.0 Gecko/20100101 Firefox/21.0"
                                -nv
                                -O ${CMAKE_CURRENT_BINARY_DIR}/face2.jpg
                                https://thispersondoesnotexist.com/image
    )

    add_executable(face_scaling_tests
                   face_locations_tests.cpp
                   face_similarity_tests.cpp
                   issues.cpp
                   prerequisites.cpp
                   utils.cpp
                   utils.hpp
    )

    target_link_libraries(face_scaling_tests
                            PRIVATE
                                ${GTEST_LIBRARY}
                                ${GTEST_MAIN_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                dlib_wrapper
                                Qt5::Core
                                Qt5::Gui
    )

    target_include_directories(face_scaling_tests
                                PRIVATE
                                    ${GTEST_INCLUDE_DIRS}
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${Qt5Gui_INCLUDE_DIRS}
    )

    target_compile_definitions(face_scaling_tests
                                PRIVATE
                                    FACES_DIR=${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(face_images
                      DEPENDS face1.jpg
                              face2.jpg
    )

    add_dependencies(face_scaling_tests face_images)
    add_test(NAME dlib_face_scaling
             COMMAND face_scaling_tests)

    set_tests_properties(dlib_face_scaling PROPERTIES LABELS "LearningTest")

endif()
