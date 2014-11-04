
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary 2.0 REQUIRED COMPONENTS QtExt)
find_package(Boost REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports/ ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Qt5)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${Boost_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GUI_SOURCES
    Qt5/gui.cpp
    Qt5/data/thread_multiplexer.cpp
    Qt5/data/photos_collector.cpp
    Qt5/model_view/db_data_model.cpp
    Qt5/model_view/model_helpers/idx_data_manager.cpp
    Qt5/model_view/model_helpers/idx_data.cpp
    Qt5/model_view/model_helpers/idxdata_deepfetcher.cpp
    Qt5/model_view/model_helpers/photos_matcher.cpp
    Qt5/model_view/view_helpers/data.cpp
    Qt5/model_view/view_helpers/positions_calculator.cpp
    Qt5/model_view/view_helpers/positions_reseter.cpp
    Qt5/model_view/images_tree_view.cpp
    Qt5/model_view/tree_item_delegate.cpp
    Qt5/components/photos_data_model.cpp
    Qt5/components/staged_photos_data_model.cpp
    Qt5/components/staged_photos_widget.cpp
    Qt5/components/filters_editor/level_editor.cpp
    Qt5/components/project_creator/project_creator_dialog.cpp
    Qt5/components/tag_editor/tag_editor_widget.cpp
    Qt5/components/tag_editor/helpers/tags_view.cpp
    Qt5/components/tag_editor/helpers/tags_model.cpp
    Qt5/ui/mainwindow.cpp
    Qt5/ui/photos_view.cpp
    )

set(GUI_HEADERS
    gui.hpp
    Qt5/data/thread_multiplexer.hpp
    Qt5/data/photos_collector.hpp
    Qt5/model_view/db_data_model.hpp
    Qt5/model_view/model_helpers/idx_data_manager.hpp
    Qt5/model_view/model_helpers/idx_data.hpp
    Qt5/model_view/model_helpers/idxdata_deepfetcher.hpp
    Qt5/model_view/model_helpers/photos_matcher.hpp
    Qt5/model_view/view_helpers/data.hpp
    Qt5/model_view/view_helpers/positions_calculator.hpp
    Qt5/model_view/view_helpers/positions_reseter.hpp
    Qt5/model_view/images_tree_view.hpp
    Qt5/model_view/tree_item_delegate.hpp
    Qt5/components/photos_data_model.hpp
    Qt5/components/staged_photos_data_model.hpp
    Qt5/components/staged_photos_widget.hpp
    Qt5/components/filters_editor/level_editor.hpp
    Qt5/components/project_creator/project_creator_dialog.hpp
    Qt5/components/tag_editor/tag_editor_widget.hpp
    Qt5/components/tag_editor/helpers/tags_view.hpp
    Qt5/components/tag_editor/helpers/tags_model.hpp
    Qt5/ui/mainwindow.hpp
    Qt5/ui/photos_view.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    Qt5/data/thread_multiplexer.hpp
    Qt5/data/photos_collector.hpp
    Qt5/components/tag_editor/tag_editor_widget.hpp
    Qt5/components/tag_editor/helpers/tags_model.hpp
    Qt5/components/staged_photos_widget.hpp
    Qt5/model_view/db_data_model.hpp
    Qt5/model_view/images_tree_view.hpp
    Qt5/model_view/model_helpers/idx_data_manager.hpp
    Qt5/model_view/model_helpers/idxdata_deepfetcher.hpp
    Qt5/ui/mainwindow.hpp
    Qt5/ui/photos_view.hpp
    )

set(GUI_RESOURCE_FILES Qt5/images.qrc)

set(GUI_UI_FILES Qt5/ui/mainwindow.ui
                 Qt5/ui/photos_view.ui)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})
qt5_wrap_ui(GUI_WRAPPED_UI ${GUI_UI_FILES})
qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS} ${GUI_WRAPPED_UI} ${GUI_RESOURCES})
target_link_libraries(gui PRIVATE analyzer core database system ${OPENLIBRARY_LIBRARIES} Qt5::Gui Qt5::Widgets)

generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
turnOnCpp11(gui)
hideSymbols(gui)

install(TARGETS gui DESTINATION ${PATH_LIBS})

if(BUILD_TESTS)

    qt5_wrap_cpp(GUI_WRAPPED_CPP_FOR_TEST Qt5/model_view/db_data_model.hpp)

    addTestTarget(gui  SOURCES
                           Qt5/data/photo_info_updater.cpp
                           Qt5/model_view/model_helpers/idx_data_manager.cpp
                           Qt5/model_view/model_helpers/idxdata_deepfetcher.cpp
                           Qt5/model_view/db_data_model.cpp
                           tests/db_data_model_tests.cpp
                           ${GUI_WRAPPED_CPP_FOR_TEST}
                       TEST_LIBRARY
                           GMOCK
       )
       
    target_link_libraries(gui_test gui system Qt5::Core)

    turnOnCpp11(gui_test)
endif(BUILD_TESTS)

