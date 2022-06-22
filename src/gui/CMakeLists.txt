
find_package(Qt6 REQUIRED COMPONENTS Widgets QuickControls2)

include(GenerateExportHeader)

if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DGUI_STATIC)
endif()

add_subdirectory(desktop/config_tabs)
add_subdirectory(desktop/models)
add_subdirectory(desktop/QmlItems)
add_subdirectory(desktop/quick_items)
add_subdirectory(desktop/ui)
add_subdirectory(desktop/ui_utils)
add_subdirectory(desktop/utils)
add_subdirectory(desktop/widgets)

set(GUI_SOURCES
    desktop/gui.cpp
)

set(GUI_HEADERS
    features.hpp
    gui.hpp
    inotifications.hpp
    desktop/config_keys.hpp
    images/images.hpp
)

set(GUI_RESOURCE_FILES images/images.qrc)

source_group(interface FILES gui.hpp)
source_group(desktop .*\/desktop\/.* )
source_group(desktop\\images_model_view .*\/images_model_view\/.* )
source_group(desktop\\ui .*\/ui\/.* )
source_group(generated_files \(moc.*\)|\(ui_.*\)|\(qrc_.*\) )

qt6_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui
            ${GUI_SOURCES}
            ${GUI_HEADERS}
            ${GUI_RESOURCES}
)

target_link_libraries(gui
    PRIVATE
        gui_config_tabs
        gui_models
        gui_utils
        gui_ui
        gui_ui_utils
        gui_widgets
        qml_itemsplugin
        quick_itemsplugin

        core
        database
        face_recognition
        photos_crawler
        project_utils
        system
)

target_include_directories(gui
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/desktop
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(BUILD_UPDATER)
    target_link_libraries(gui PRIVATE updater)
endif()

if(WIN32)
    target_link_libraries(gui PRIVATE Qt::QuickControls2)
endif()

generate_export_header(gui)
hideSymbols(gui)

if(BUILD_SHARED_LIBS)
    install(TARGETS gui RUNTIME DESTINATION ${PATH_LIBS}
                        LIBRARY DESTINATION ${PATH_LIBS})

    objdump_target(gui)
endif()

if(BUILD_TESTING)
    include(gui_tests.cmake)
endif()
