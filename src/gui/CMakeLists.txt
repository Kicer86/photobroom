
project(gui)

find_package(Qt5Widgets REQUIRED)
find_package(OpenLibrary REQUIRED COMPONENTS QtExt palgorithm)
find_package(OpenMP REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports/ ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS} )
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GUI_SOURCES
    Qt5/gui.cpp
    Qt5/initializator.cpp
    Qt5/mainwindow.cpp
    Qt5/centralwidget.cpp
    Qt5/main_view_widget.cpp
    Qt5/photo_info.cpp
    Qt5/photo_loader.cpp
    Qt5/photos_view_widget.cpp
    Qt5/photos_staging_area.cpp
    Qt5/thread_multiplexer.cpp
    Qt5/private/preparing_photos_widget_private.cpp
    Qt5/model_view/images_model.cpp
    Qt5/model_view/images_view.cpp
    Qt5/model_view/image_manager.cpp
    Qt5/model_view/positions_cache.cpp
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
    Qt5/iphoto_loader.hpp
    Qt5/mainwindow.hpp
    Qt5/centralwidget.hpp
    Qt5/main_view_widget.hpp
    Qt5/photo_info.hpp
    Qt5/photo_loader.hpp
    Qt5/photos_view_widget.hpp
    Qt5/photos_staging_area.hpp
    Qt5/thread_multiplexer.hpp
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/model_view/images_model.hpp
    Qt5/model_view/images_view.hpp
    Qt5/model_view/image_manager.hpp
    Qt5/model_view/positions_cache.hpp
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
    Qt5/photos_staging_area.hpp
    Qt5/photos_view_widget.hpp
    Qt5/thread_multiplexer.hpp
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/widgets/tag_editor/tag_editor_widget.hpp
    Qt5/widgets/tag_editor/tag_definition.hpp
    Qt5/widgets/tag_editor/tag_entry.hpp
    Qt5/widgets/tag_editor/ivalue_widget.hpp
    )

set(GUI_RESOURCE_FILES Qt5/images/images.qrc)

if(BUILD_TESTS)
    find_package(GTest)
    include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
    set(GUI_SOURCES
        ${GUI_SOURCES}
       )
endif(BUILD_TESTS)

#photo loader uses openmp
addSourceFlags(Qt5/photo_loader.cpp COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})
qt5_add_resources(GUI_RESOURCES ${GUI_RESOURCE_FILES})

add_library(gui SHARED ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS} ${GUI_RESOURCES})
target_link_libraries(gui core ${OPENLIBRARY_QTEXT_LIBRARIES} ${OPENLIBRARY_PALGORITHM_LIBRARIES})
qt5_use_modules(gui Widgets)
addFlags(gui LINK_FLAGS ${OpenMP_CXX_FLAGS})


generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
target_link_libraries(gui analyzer database)
turnOnCpp11(gui)


if(BUILD_TESTS)
    enableCodeCoverage(gui)
    enableGTest(gui GTEST)
endif(BUILD_TESTS)
