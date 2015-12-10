
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui 5.5 REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(OpenLibrary REQUIRED putils)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/gui
                    ${CMAKE_SOURCE_DIR}/src/gui/desktop
                    $<TARGET_PROPERTY:configuration,INTERFACE_INCLUDE_DIRECTORIES>)

include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS})


if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DGUI_STATIC)
endif()

add_subdirectory(desktop/config_tabs)
add_subdirectory(desktop/models)
add_subdirectory(desktop/utils)
add_subdirectory(desktop/widgets)

set(GUI_SOURCES
    desktop/gui.cpp
    desktop/components/configurable_tree_item_delegate.cpp
    desktop/views/view_impl/data.cpp
    desktop/views/view_impl/model_index_info.cpp
    desktop/views/view_impl/positions_calculator.cpp
    desktop/views/view_impl/positions_reseter.cpp
    desktop/views/images_tree_view.cpp
    desktop/views/tree_item_delegate.cpp
    desktop/ui/configuration_dialog.cpp
    desktop/ui/mainwindow.cpp
    desktop/ui/photos_add_dialog.cpp
    desktop/ui/project_picker.cpp
    )

set(GUI_HEADERS
    gui.hpp
    desktop/components/configurable_tree_item_delegate.hpp
    desktop/config_keys.hpp
    desktop/ithread_multiplexer.hpp
    desktop/views/view_impl/data.hpp
    desktop/views/view_impl/model_index_info.hpp
    desktop/views/view_impl/positions_calculator.hpp
    desktop/views/view_impl/positions_reseter.hpp
    desktop/views/view_impl/view_data_set.hpp
    desktop/views/images_tree_view.hpp
    desktop/views/tree_item_delegate.hpp
    desktop/ui/configuration_dialog.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_add_dialog.hpp
    desktop/ui/project_picker.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    desktop/views/images_tree_view.hpp
    desktop/views/view_impl/view_data_set.hpp
    desktop/ui/configuration_dialog.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_add_dialog.hpp
    desktop/ui/project_picker.hpp
    )

set(GUI_RESOURCE_FILES images/images.qrc)

set(GUI_UI_FILES
                 desktop/ui/configuration_dialog.ui
                 desktop/ui/mainwindow.ui
                 desktop/ui/photos_add_dialog.ui
                 desktop/ui/project_picker.ui
    )

source_group(interface FILES gui.hpp)
source_group(desktop .*\/desktop\/.* )
source_group(desktop\\images_model_view .*\/images_model_view\/.* )
source_group(desktop\\ui .*\/ui\/.* )
source_group(generated_files \(moc.*\)|\(ui_.*\)|\(qrc_.*\) )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})
qt5_wrap_ui(GUI_WRAPPED_UI ${GUI_UI_FILES})
qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui
            ${GUI_SOURCES}
            ${GUI_WRAPPED_CPP}
            ${GUI_HEADERS}
            ${GUI_WRAPPED_UI}
            ${GUI_RESOURCES}
            $<TARGET_OBJECTS:gui_models>
            $<TARGET_OBJECTS:gui_utils>
            $<TARGET_OBJECTS:gui_widgets>
            $<TARGET_OBJECTS:gui_config_tabs>
           )

target_link_libraries(gui PRIVATE photos_crawler configuration core database system updater ${OPENLIBRARY_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Svg)
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(gui)
hideSymbols(gui)

if(BUILD_SHARED_LIBS)
    install(TARGETS gui RUNTIME DESTINATION ${PATH_LIBS}
                        LIBRARY DESTINATION ${PATH_LIBS})
endif()
