
project(qtgui)

find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_BINARY_DIR} .)

set(QtGUI_SOURCES 
    initializator.cpp    
    )

set(QtGUI_HEADERS
    initializator.hpp
    )

source_group(files REGULAR_EXPRESSION .*Qt.* )

add_library(QtGui SHARED ${QtGUI_SOURCES} ${QtGUI_HEADERS})
qt5_use_modules(QtGui Widgets)

exportSymbols(QtGui)
hideSymbols(QtGui)
turnOnCpp11(QtGui)
turnOnAllWarnings(${QtGUI_SOURCES})
