

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

    # download face dataset from vis-www.cs.umass.edu/lfw/
    # another datasets: https://www.researchgate.net/post/Does_anyone_know_of_a_downloadable_large_faces_dataset
    add_custom_command(OUTPUT lfw.tgz
                       COMMAND ${WGET} http://vis-www.cs.umass.edu/lfw/lfw.tgz
                                    -O ${CMAKE_CURRENT_BINARY_DIR}/lfw.tgz
    )

    add_custom_command(OUTPUT lfw/George_W_Bush/George_W_Bush_0518.jpg
                       COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_BINARY_DIR}/lfw.tgz
                       DEPENDS lfw.tgz
    )

    add_executable(dlib_behaviour_tests
                   face_locations_tests.cpp
                   face_similarity_tests.cpp
                   issues.cpp
                   person_recognition_tests.cpp
                   prerequisites.cpp
                   utils.cpp
                   utils.hpp
    )

    target_link_libraries(dlib_behaviour_tests
                            PRIVATE
                                ${GTEST_LIBRARY}
                                ${GTEST_MAIN_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                core
                                dlib_wrapper
                                Qt5::Core
                                Qt5::Gui
    )

    target_include_directories(dlib_behaviour_tests
                                PRIVATE
                                    ${GTEST_INCLUDE_DIRS}
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${Qt5Gui_INCLUDE_DIRS}
    )

    target_compile_definitions(dlib_behaviour_tests
                                PRIVATE
                                    FACES_DIR=${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(face_images
                      DEPENDS face1.jpg
                              face2.jpg
                              lfw/George_W_Bush/George_W_Bush_0518.jpg
    )

    add_dependencies(dlib_behaviour_tests face_images)
    add_test(NAME dlib_behaviour
             COMMAND dlib_behaviour_tests)

    set_tests_properties(dlib_behaviour PROPERTIES LABELS "LearningTest")

endif()
