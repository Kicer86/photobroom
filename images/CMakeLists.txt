
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/images/main_ico.png
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/images
                   COMMAND img_manip ${CMAKE_SOURCE_DIR}/images/main_ico.svg ${CMAKE_BINARY_DIR}/images/main_ico.png
                   DEPENDS img_manip
                  )

add_custom_target(Images ALL
                  DEPENDS ${CMAKE_BINARY_DIR}/images/main_ico.png
                 )
