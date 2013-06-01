

project(gui)

add_subdirectory(implementation/Qt)

include_directories(${CMAKE_BINARY_DIR} .)

set(GUI_SOURCES
    implementation/gui.cpp
    )

set(Gui_HEADERS
    gui.hpp
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui STATIC ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(gui)

exportSymbols(gui)
hideSymbols(gui)
turnOnCpp11(gui)
turnOnAllWarnings(${GUI_SOURCES})
