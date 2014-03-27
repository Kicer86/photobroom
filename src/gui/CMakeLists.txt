
project(gui)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary REQUIRED COMPONENTS QtExt)
find_package(Boost REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports/ ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Qt5)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GUI_SOURCES
    Qt5/gui.cpp
    Qt5/initializator.cpp
    Qt5/mainwindow.cpp
    Qt5/centralwidget.cpp
    Qt5/data/photo_info.cpp
    Qt5/data/thread_multiplexer.cpp
    Qt5/private/preparing_photos_widget_private.cpp
    Qt5/model_view/db_data_model.cpp
    Qt5/model_view/images_model.cpp
    Qt5/model_view/images_view.cpp
    Qt5/model_view/image_manager.cpp
    Qt5/model_view/positions_cache.cpp
    Qt5/widgets/browse_view/main_view_widget.cpp
    Qt5/widgets/photos_view_widget.cpp
    Qt5/widgets/photos_staging/photos_staging_area.cpp
    Qt5/widgets/tag_editor/tag_editor_widget.cpp
    Qt5/widgets/tag_editor/tag_definition.cpp
    Qt5/widgets/tag_editor/tag_entry.cpp
    Qt5/widgets/tag_editor/entries_manager.cpp
    Qt5/widgets/tag_editor/ivalue_widget.cpp
    Qt5/widgets/tag_editor/converter.cpp
    Qt5/widgets/tag_editor/text_widget.cpp
    )

set(GUI_HEADERS
    gui.hpp
    Qt5/initializator.hpp
    Qt5/mainwindow.hpp
    Qt5/centralwidget.hpp
    Qt5/data/photo_info.hpp
    Qt5/data/thread_multiplexer.hpp
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/model_view/db_data_model.hpp
    Qt5/model_view/images_model.hpp
    Qt5/model_view/images_view.hpp
    Qt5/model_view/image_manager.hpp
    Qt5/model_view/positions_cache.hpp
    Qt5/widgets/browse_view/main_view_widget.hpp
    Qt5/widgets/photos_view_widget.hpp
    Qt5/widgets/photos_staging/photos_staging_area.hpp
    Qt5/widgets/tag_editor/tag_editor_widget.hpp
    Qt5/widgets/tag_editor/tag_definition.hpp
    Qt5/widgets/tag_editor/tag_entry.hpp
    Qt5/widgets/tag_editor/entries_manager.hpp
    Qt5/widgets/tag_editor/ivalue_widget.hpp
    Qt5/widgets/tag_editor/converter.hpp
    Qt5/widgets/tag_editor/text_widget.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    Qt5/data/thread_multiplexer.hpp
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/widgets/photos_view_widget.hpp
    Qt5/widgets/photos_staging/photos_staging_area.hpp
    Qt5/widgets/tag_editor/tag_editor_widget.hpp
    Qt5/widgets/tag_editor/tag_definition.hpp
    Qt5/widgets/tag_editor/tag_entry.hpp
    Qt5/widgets/tag_editor/ivalue_widget.hpp
    )

set(GUI_RESOURCE_FILES Qt5/images/images.qrc)

if(BUILD_TESTS)
    addTestTarget(gui  SOURCES
                           Qt5/model_view/db_data_model.cpp
                           tests/db_data_model_tests.cpp
                       TEST_LIBRARY
                           GMOCK
       )
    target_link_libraries(gui_test gui)
    qt5_use_modules(gui_test Core)

    turnOnCpp11(gui_test)
endif(BUILD_TESTS)


source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})
qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS} ${GUI_RESOURCES})
target_link_libraries(gui core ${OPENLIBRARY_QTEXT_LIBRARIES})
qt5_use_modules(gui Widgets Gui)

generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
target_link_libraries(gui analyzer database)
turnOnCpp11(gui)
hideSymbols(gui)
