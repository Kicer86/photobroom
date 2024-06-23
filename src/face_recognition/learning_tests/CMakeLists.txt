
if(FALSE)                                                   # TODO: reenable when dlib-cuda starts working on Arch
    find_program(WGET wget REQUIRED)

    find_package(GTest REQUIRED CONFIG)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui)

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
                   issues.cpp
                   person_recognition_tests.cpp
                   scaled_face_similarity_tests.cpp
                   utils.cpp
                   utils.hpp
    )

    target_link_libraries(dlib_behaviour_tests
                            PRIVATE
                                GTest::gtest
                                GTest::gtest_main
                                core
                                dlib_wrapper
                                Qt::Core
                                Qt::Gui
    )

    target_include_directories(dlib_behaviour_tests
                                PRIVATE
                                    ${GTEST_INCLUDE_DIRS}
    )

    target_compile_definitions(dlib_behaviour_tests
                                PRIVATE
                                    FACES_DIR=${CMAKE_CURRENT_BINARY_DIR}
    )

    find_package(dlib REQUIRED)
    if (${dlib_VERSION} VERSION_LESS 19.25)
        target_compile_definitions(dlib_behaviour_tests PRIVATE DLIB_HAS_CUDA_MEMORY_OVERFLOW_BUG)
    endif()

    add_custom_target(face_images
                      DEPENDS lfw/George_W_Bush/George_W_Bush_0518.jpg
    )

    add_dependencies(dlib_behaviour_tests face_images)
    add_test(NAME dlib_behaviour
             COMMAND dlib_behaviour_tests)

    set_tests_properties(dlib_behaviour PROPERTIES LABELS "LearningTest")

endif()
