

project(gui)

add_subdirectory(implementation/Qt)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/exports/ .)

set(GUI_SOURCES
    implementation/gui.cpp
    )

set(Gui_HEADERS
    gui.hpp
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui SHARED ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(gui QtGui)

generate_export_header(gui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/gui_export.h)
turnOnCpp11(gui)
turnOnAllWarnings(${GUI_SOURCES})
