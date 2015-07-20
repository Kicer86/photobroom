
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Svg REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/gui
                    ${CMAKE_SOURCE_DIR}/src/gui/desktop)

include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS})


if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DGUI_STATIC)
endif()

add_subdirectory(desktop/widgets)
add_subdirectory(desktop/utils)

set(GUI_SOURCES
    desktop/gui.cpp
    desktop/images_model_view/db_data_model.cpp
    desktop/images_model_view/model_helpers/idx_data_manager.cpp
    desktop/images_model_view/model_helpers/idx_data.cpp
    desktop/images_model_view/model_helpers/idxdata_deepfetcher.cpp
    desktop/images_model_view/model_helpers/photos_matcher.cpp
    desktop/images_model_view/view_helpers/data.cpp
    desktop/images_model_view/view_helpers/model_index_info.cpp
    desktop/images_model_view/view_helpers/positions_calculator.cpp
    desktop/images_model_view/view_helpers/positions_reseter.cpp
    desktop/images_model_view/images_tree_view.cpp
    desktop/images_model_view/tree_item_delegate.cpp
    desktop/ui/mainwindow.cpp
    desktop/ui/photos_add_dialog.cpp
    desktop/ui/photos_view.cpp
    desktop/ui/project_picker.cpp
    )

set(GUI_HEADERS
    gui.hpp
    desktop/images_model_view/db_data_model.hpp
    desktop/images_model_view/model_helpers/idx_data_manager.hpp
    desktop/images_model_view/model_helpers/idx_data.hpp
    desktop/images_model_view/model_helpers/idxdata_deepfetcher.hpp
    desktop/images_model_view/model_helpers/photos_matcher.hpp
    desktop/images_model_view/view_helpers/data.hpp
    desktop/images_model_view/view_helpers/model_index_info.hpp
    desktop/images_model_view/view_helpers/positions_calculator.hpp
    desktop/images_model_view/view_helpers/positions_reseter.hpp
    desktop/images_model_view/view_helpers/view_data_set.hpp
    desktop/images_model_view/images_tree_view.hpp
    desktop/images_model_view/tree_item_delegate.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_add_dialog.hpp
    desktop/ui/photos_view.hpp
    desktop/ui/project_picker.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    desktop/images_model_view/images_tree_view.hpp
    desktop/images_model_view/model_helpers/idx_data_manager.hpp
    desktop/images_model_view/model_helpers/idxdata_deepfetcher.hpp
    desktop/images_model_view/view_helpers/view_data_set.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_add_dialog.hpp
    desktop/ui/photos_view.hpp
    desktop/ui/project_picker.hpp
    )

set(GUI_RESOURCE_FILES desktop/images.qrc)

set(GUI_UI_FILES desktop/ui/mainwindow.ui
                 desktop/ui/photos_add_dialog.ui
                 desktop/ui/photos_view.ui
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
            $<TARGET_OBJECTS:gui_widgets>
            $<TARGET_OBJECTS:gui_utils>
           )

target_link_libraries(gui PRIVATE photos_crawler configuration core database system updater ${OPENLIBRARY_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Svg)
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(gui)
hideSymbols(gui)

if(BUILD_SHARED_LIBS)
    install(TARGETS gui RUNTIME DESTINATION ${PATH_LIBS}
                        LIBRARY DESTINATION ${PATH_LIBS})
endif()
