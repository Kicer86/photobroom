
project(database_tools CXX)

find_package(Qt5Core REQUIRED)
find_package(OpenLibrary REQUIRED utils)
find_package(Qt5Gui REQUIRED)

set(DATABASE_TOOLS_SOURCES
        implementation/photos_analyzer.cpp
        implementation/photo_info_updater.cpp
    )

set(DATABASE_TOOLS_HEADERS
        photos_analyzer.hpp
        implementation/photo_info_updater.hpp
   )

set(DATABASE_TOOLS_MOC photos_analyzer.hpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDE_DIRS})

qt5_wrap_cpp(DATABASE_TOOLS_MOCED ${DATABASE_TOOLS_MOC})

add_library(database_tools ${DATABASE_TOOLS_SOURCES} ${DATABASE_TOOLS_HEADERS} ${DATABASE_TOOLS_MOCED})
target_link_libraries(database_tools core)

qt5_use_modules(database_tools Core Gui)

generate_export_header(database_tools EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_tools_export.h)
turnOnCpp11(database_tools)
hideSymbols(database_tools)

install(TARGETS database_tools DESTINATION ${PATH_LIBS})
