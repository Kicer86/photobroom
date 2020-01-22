

find_program(WGET wget)

if(WGET)

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

    add_custom_target(face_recognition_learning_tests
                      DEPENDS face1.jpg
                              face2.jpg
    )

endif()
