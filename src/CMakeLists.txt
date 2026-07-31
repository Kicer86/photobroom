
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(QtClick QUIET)

include(GenerateExportHeader)

configure_file(config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)

if(BUILD_UPDATER)
    add_definitions(-DUPDATER_ENABLED)
    add_subdirectory(updater)
endif()

add_subdirectory(third_party)

add_subdirectory(core)
add_subdirectory(crash_catcher)
add_subdirectory(photos_crawler)
add_subdirectory(database)
add_subdirectory(face_recognition)
add_subdirectory(gui)
add_subdirectory(plugins)
add_subdirectory(project_utils)
add_subdirectory(unit_tests_utils)
add_subdirectory(system)

add_executable(photo_broom
    config_storage.cpp
    config_storage.hpp
    features.hpp
    main.cpp
    $<TARGET_OBJECTS:plugins>
)

if(WIN32)

    set(MAIN_ICO_PATH ${CMAKE_CURRENT_BINARY_DIR}/main_ico.ico)

    add_custom_command(
        OUTPUT ${MAIN_ICO_PATH}
        COMMAND
            ${CMAKE_COMMAND} -E env
            "PATH=$<TARGET_FILE_DIR:Qt6::Core>"
            "QT_PLUGIN_PATH=${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}"
            $<TARGET_FILE:svg_to_ico>
            ${CMAKE_SOURCE_DIR}/images/photo_broom.svg
            ${MAIN_ICO_PATH}
            32
            32
        DEPENDS
            svg_to_ico
            ${CMAKE_SOURCE_DIR}/images/photo_broom.svg
        VERBATIM
    )

    set_target_properties(photo_broom PROPERTIES WIN32_EXECUTABLE TRUE)

    target_sources(photo_broom
        PRIVATE
            env/PhotoBroom.rc
            ${MAIN_ICO_PATH}
    )

    # used by env/PhotoBroom.rc
    configure_file(env/PhotoBroom.hpp.cmake PhotoBroom.hpp @ONLY)

endif()

target_link_libraries(photo_broom
    core
    crash_catcher
    database
    face_recognition
    gui
    project_utils
    system
    $<$<TARGET_EXISTS:QtClick::QtClick>:QtClick::QtClick>
    Qt::Core
    Qt::Widgets
)

target_include_directories(photo_broom
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

install(TARGETS photo_broom RUNTIME DESTINATION ${PATH_BIN})

objdump_target(photo_broom)
