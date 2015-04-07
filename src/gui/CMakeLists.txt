
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

set(GUI_SOURCES
    desktop/gui.cpp
    desktop/data/thread_multiplexer.cpp
    desktop/data/photos_collector.cpp
    desktop/model_view/db_data_model.cpp
    desktop/model_view/model_helpers/idx_data_manager.cpp
    desktop/model_view/model_helpers/idx_data.cpp
    desktop/model_view/model_helpers/idxdata_deepfetcher.cpp
    desktop/model_view/model_helpers/photos_matcher.cpp
    desktop/model_view/view_helpers/data.cpp
    desktop/model_view/view_helpers/model_index_info.cpp
    desktop/model_view/view_helpers/positions_calculator.cpp
    desktop/model_view/view_helpers/positions_reseter.cpp
    desktop/model_view/images_tree_view.cpp
    desktop/model_view/tree_item_delegate.cpp
    desktop/components/photos_data_model.cpp
    desktop/components/photos_widget.cpp
    desktop/components/staged_photos_data_model.cpp
    desktop/components/staged_photos_widget.cpp
    desktop/components/filters_editor/level_editor.cpp
    desktop/components/project_creator/project_creator_dialog.cpp
    desktop/components/tag_editor/tag_editor_widget.cpp
    desktop/components/tag_editor/helpers/tags_operator.cpp
    desktop/components/tag_editor/helpers/tags_view.cpp
    desktop/components/tag_editor/helpers/tags_view_delegate.cpp
    desktop/components/tag_editor/helpers/tags_model.cpp
    desktop/components/tag_editor/helpers/tag_value_widget_factory.cpp
    desktop/ui/mainwindow.cpp
    desktop/ui/photos_view.cpp
    desktop/ui/project_picker.cpp
    )

set(GUI_HEADERS
    gui.hpp
    desktop/data/thread_multiplexer.hpp
    desktop/data/photos_collector.hpp
    desktop/model_view/db_data_model.hpp
    desktop/model_view/model_helpers/idx_data_manager.hpp
    desktop/model_view/model_helpers/idx_data.hpp
    desktop/model_view/model_helpers/idxdata_deepfetcher.hpp
    desktop/model_view/model_helpers/photos_matcher.hpp
    desktop/model_view/view_helpers/data.hpp
    desktop/model_view/view_helpers/model_index_info.hpp
    desktop/model_view/view_helpers/positions_calculator.hpp
    desktop/model_view/view_helpers/positions_reseter.hpp
    desktop/model_view/view_helpers/view_data_set.hpp
    desktop/model_view/images_tree_view.hpp
    desktop/model_view/tree_item_delegate.hpp
    desktop/components/iview.hpp
    desktop/components/photos_data_model.hpp
    desktop/components/photos_widget.hpp
    desktop/components/staged_photos_data_model.hpp
    desktop/components/staged_photos_widget.hpp
    desktop/components/filters_editor/level_editor.hpp
    desktop/components/project_creator/project_creator_dialog.hpp
    desktop/components/tag_editor/tag_editor_widget.hpp
    desktop/components/tag_editor/helpers/itags_operator.hpp
    desktop/components/tag_editor/helpers/tags_operator.hpp
    desktop/components/tag_editor/helpers/tags_view.hpp
    desktop/components/tag_editor/helpers/tags_view_delegate.hpp
    desktop/components/tag_editor/helpers/tags_model.hpp
    desktop/components/tag_editor/helpers/tag_value_widget_factory.hpp
    desktop/components/tag_editor/helpers/tag_value_widgets.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_view.hpp
    desktop/ui/project_picker.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    desktop/data/thread_multiplexer.hpp
    desktop/data/photos_collector.hpp
    desktop/components/filters_editor/level_editor.hpp
    desktop/components/project_creator/project_creator_dialog.hpp
    desktop/components/staged_photos_widget.hpp
    desktop/components/tag_editor/tag_editor_widget.hpp
    desktop/components/tag_editor/helpers/tags_model.hpp
    desktop/components/tag_editor/helpers/tag_value_widgets.hpp
    desktop/model_view/images_tree_view.hpp
    desktop/model_view/model_helpers/idx_data_manager.hpp
    desktop/model_view/model_helpers/idxdata_deepfetcher.hpp
    desktop/model_view/view_helpers/view_data_set.hpp
    desktop/ui/mainwindow.hpp
    desktop/ui/photos_view.hpp
    desktop/ui/project_picker.hpp
    )

set(GUI_RESOURCE_FILES desktop/images.qrc)

set(GUI_UI_FILES desktop/ui/mainwindow.ui
                 desktop/ui/photos_view.ui
                 desktop/ui/project_picker.ui
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})
qt5_wrap_ui(GUI_WRAPPED_UI ${GUI_UI_FILES})
qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS} ${GUI_WRAPPED_UI} ${GUI_RESOURCES})
target_link_libraries(gui PRIVATE photos_crawler configuration core database system ${OPENLIBRARY_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Svg)
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(gui)
hideSymbols(gui)

if(BUILD_SHARED_LIBS)
    install(TARGETS gui RUNTIME DESTINATION ${PATH_LIBS}
                        LIBRARY DESTINATION ${PATH_LIBS})
else()
    add_definitions(-DGUI_STATIC)
endif()
