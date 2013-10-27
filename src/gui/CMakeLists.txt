
project(gui)

find_package(Qt5Widgets REQUIRED)
find_package(OpenLibrary REQUIRED COMPONENTS QtExt)

include(GenerateExportHeader)

include_directories(${OPENLIBRARY_INCLUDES} ${CMAKE_BINARY_DIR}/exports/ ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GUI_SOURCES
    Qt5/gui.cpp
    Qt5/initializator.cpp
    Qt5/mainwindow.cpp
    Qt5/centralwidget.cpp
    Qt5/main_view_widget.cpp
    Qt5/photos_view_widget.cpp
    Qt5/preparing_photos_widget.cpp
    Qt5/private/preparing_photos_widget_private.cpp
    Qt5/data/photo_info.cpp
    Qt5/widgets/tag_editor/tag_editor_widget.cpp
    Qt5/widgets/tag_editor/tag_definition.cpp
    Qt5/widgets/tag_editor/tag_entry.cpp
    )

set(GUI_HEADERS 
    gui.hpp
    Qt5/initializator.hpp
    Qt5/mainwindow.hpp
    Qt5/centralwidget.hpp
    Qt5/main_view_widget.hpp
    Qt5/photos_view_widget.hpp
    Qt5/preparing_photos_widget.hpp
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/data/photo_info.hpp
    Qt5/widgets/tag_editor/tag_editor_widget.hpp
    Qt5/widgets/tag_editor/tag_definition.hpp
    Qt5/widgets/tag_editor/tag_entry.hpp
    )

#set of file to be moced
set(GUI_FILES_TO_MOC
    Qt5/private/preparing_photos_widget_private.hpp
    Qt5/preparing_photos_widget.hpp
    Qt5/widgets/tag_editor/tag_editor_widget.hpp
    Qt5/photos_view_widget.hpp
    Qt5/widgets/tag_editor/tag_definition.hpp
    Qt5/widgets/tag_editor/tag_entry.hpp
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_FILES_TO_MOC})

add_library(gui SHARED ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS})
target_link_libraries(gui core ${OPENLIBRARY_QTEXT_LIBRARIES})
qt5_use_modules(gui Widgets)

generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
target_link_libraries(gui analyzer)
turnOnCpp11(gui)
turnOnAllWarnings(${GUI_SOURCES})
