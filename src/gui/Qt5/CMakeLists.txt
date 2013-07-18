
project(qtgui)



include(GenerateExportHeader)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/exports/ .)

set(QtGUI_SOURCES 
    initializator.cpp
    )

set(QtGUI_HEADERS
    initializator.hpp
    )

#set(QtGUI_UI 
#    ui/mainwindow.ui
#   )

source_group(files REGULAR_EXPRESSION .*Qt.* )

qt5_wrap_ui(QtGUI_WRAPPED_UI ${QtGUI_UI})

add_library(QtGui SHARED ${QtGUI_SOURCES} ${QtGUI_WRAPPED_UI} ${QtGUI_HEADERS})
qt5_use_modules(QtGui Widgets)

generate_export_header(QtGui EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/qtgui_export.h)

turnOnCpp11(QtGui)
turnOnAllWarnings(${QtGUI_SOURCES})
