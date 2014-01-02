
project(MySQL_Backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS})

set(BACKEND_SOURCES mysql_server.cpp backend.cpp ../sql_backend.cpp)

set(BACKEND_HEADERS mysql_server.hpp backend.hpp)

qt5_wrap_cpp(BACKEND_WRAPPED_CPP mysql_server.hpp)

add_library(database_mysql_backend SHARED ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_WRAPPED_CPP})
qt5_use_modules(database_mysql_backend Core Sql)
turnOnCpp11(database_mysql_backend)
generate_export_header(database_mysql_backend EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_mysql_backend_export.h)

