
project(gui)

find_package(Qt5Widgets REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/exports/ .)

set(GUI_SOURCES
    Qt5/gui.cpp
    Qt5/initializator.cpp
    Qt5/mainwindow.cpp
    Qt5/centralwidget.cpp
    Qt5/main_view_widget.cpp
    Qt5/preparing_photos_widget.cpp
    )

set(GUI_HEADERS
    gui.hpp
    Qt5/initializator.hpp
    )

set(GUI_HPP_FILES
    Qt5/mainwindow.hpp
    Qt5/centralwidget.hpp
    Qt5/main_view_widget.hpp
    Qt5/preparing_photos_widget.hpp
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(GUI_WRAPPED_CPP ${GUI_HPP_FILES})

add_library(gui SHARED ${GUI_SOURCES} ${GUI_WRAPPED_CPP} ${GUI_HEADERS} ${GUI_HPP_FILES})
qt5_use_modules(gui Widgets)

generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
turnOnCpp11(gui)
turnOnAllWarnings(${GUI_SOURCES})
