
project(SQLite_Backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src/database/)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS})

set(BACKEND_SOURCES backend.cpp)
set(BACKEND_HEADERS backend.hpp)

add_library(database_sqlite_backend SHARED ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_WRAPPED_CPP})
target_link_libraries(database_sqlite_backend sql_backend_base)
qt5_use_modules(database_sqlite_backend Core Sql)
turnOnCpp11(database_sqlite_backend)
generate_export_header(database_sqlite_backend EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_sqlite_backend_export.h)
hideSymbols(database_sqlite_backend)
