
project(database_tools CXX)

set(DATABASE_TOOLS_SOURCES
        implementation/photos_updater.cpp)

set(DATABASE_TOOLS_HEADERS
        photos_updater.hpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(database_tools ${DATABASE_TOOLS_SOURCES} ${DATABASE_TOOLS_HEADERS})

generate_export_header(database_tools EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_tools_export.h)
turnOnCpp11(database_tools)
hideSymbols(database_tools)

install(TARGETS database_tools DESTINATION ${PATH_LIBS})
