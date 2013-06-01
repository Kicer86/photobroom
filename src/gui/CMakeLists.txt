

project(database)

find_package(Qt COMPONENTS QtGui QtCore)

include_directories(${CMAKE_BINARY_DIR} ${QTGUI_INCLUDES} .)

set(GUI_SOURCES
    implementation/gui.cpp
    )

set(DATABASE_HEADERS
    gui.hpp
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui STATIC ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(gui)

exportSymbols(gui)
hideSymbols(gui)
turnOnCpp11(gui)
turnOnAllWarnings(${GUI_SOURCES})
