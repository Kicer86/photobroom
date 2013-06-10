

project(qtgui)

find_package(Qt COMPONENTS QtGui QtCore)

include_directories(${CMAKE_BINARY_DIR} ${QTGUI_INCLUDES} .)

set(QtGUI_SOURCES 
    initializator.cpp    
    )

set(QtGUI_HEADERS
    initializator.hpp
    )

source_group(files REGULAR_EXPRESSION .*Qt.* )

add_library(QtGui SHARED ${QtGUI_SOURCES} ${QtGUI_HEADERS})
#target_link_libraries(QtGui)

exportSymbols(QtGui)
hideSymbols(QtGui)
turnOnCpp11(QtGui)
turnOnAllWarnings(${QtGUI_SOURCES})
