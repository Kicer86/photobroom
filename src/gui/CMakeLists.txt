
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui 5.5 REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(OpenLibrary 2.2 REQUIRED putils QtExt)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/gui
                    ${CMAKE_SOURCE_DIR}/src/gui/desktop
)

include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DGUI_STATIC)
endif()

add_subdirectory(desktop/config_tabs)
add_subdirectory(desktop/models)
add_subdirectory(desktop/ui)
add_subdirectory(desktop/ui_utils)
add_subdirectory(desktop/utils)
add_subdirectory(desktop/views)
add_subdirectory(desktop/widgets)

set(GUI_SOURCES
    desktop/gui.cpp
)

set(GUI_HEADERS
    gui.hpp
    desktop/config_keys.hpp
)

set(GUI_RESOURCE_FILES images/images.qrc)

source_group(interface FILES gui.hpp)
source_group(desktop .*\/desktop\/.* )
source_group(desktop\\images_model_view .*\/images_model_view\/.* )
source_group(desktop\\ui .*\/ui\/.* )
source_group(generated_files \(moc.*\)|\(ui_.*\)|\(qrc_.*\) )

qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui
            ${GUI_SOURCES}
            ${GUI_HEADERS}
            ${GUI_RESOURCES}
            $<TARGET_OBJECTS:gui_config_tabs>
            $<TARGET_OBJECTS:gui_models>
            $<TARGET_OBJECTS:gui_utils>
            $<TARGET_OBJECTS:gui_ui>
            $<TARGET_OBJECTS:gui_ui_utils>
            $<TARGET_OBJECTS:gui_views>
            $<TARGET_OBJECTS:gui_widgets>
)

target_link_libraries(gui
                        PRIVATE
                            core
                            database
                            photos_crawler
                            project_utils
                            system
                            updater
                            ${OPENLIBRARY_LIBRARIES}
                            Qt5::Gui
                            Qt5::Svg
                            Qt5::Widgets
)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(gui)
hideSymbols(gui)

if(BUILD_SHARED_LIBS)
    install(TARGETS gui RUNTIME DESTINATION ${PATH_LIBS}
                        LIBRARY DESTINATION ${PATH_LIBS})
endif()

if(BUILD_TESTS)
    include(gui_tests.cmake)
endif()

