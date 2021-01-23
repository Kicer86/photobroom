
find_package(Qt5 REQUIRED COMPONENTS Core)

get_filename_component(qt_bin_dir ${QT_MOC_EXECUTABLE} DIRECTORY)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/images/main_ico.ico
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/images
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${qt_bin_dir}" $<TARGET_FILE:img_manip> ${CMAKE_SOURCE_DIR}/images/photo_broom.svg ${CMAKE_BINARY_DIR}/images/main_ico.ico 32
    DEPENDS img_manip
)

add_custom_target(Images
    DEPENDS ${CMAKE_BINARY_DIR}/images/main_ico.ico
)
