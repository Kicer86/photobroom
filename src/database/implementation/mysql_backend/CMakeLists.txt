
project(MySQL_Backend)

find_package(Qt5Core REQUIRED)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})

set(BACKEND_SOURCES mysql_server.cpp backend.cpp)

set(BACKEND_HEADERS mysql_server.hpp backend.hpp)

add_library(database_mysql_backend SHARED ${BACKEND_SOURCES} ${BACKEND_HEADERS})
qt5_use_modules(database_mysql_backend Core)
turnOnCpp11(database_mysql_backend)
generate_export_header(database_mysql_backend EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_mysql_backend_export.h)

