
project(MySQL_Backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenLibrary REQUIRED)
find_package(boost 1.53.0 REQUIRED optional filesystem)

add_definitions(-DBOOST_ALL_NO_LIB)                         #prevent boost from autolinking

include_directories(${CMAKE_SOURCE_DIR}/src/
                    ${CMAKE_SOURCE_DIR}/src/database/
                    ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(BACKEND_SOURCES mysql_server.cpp backend.cpp)
set(BACKEND_HEADERS mysql_server.hpp backend.hpp)

qt5_wrap_cpp(BACKEND_WRAPPED_CPP backend.hpp mysql_server.hpp)

add_library(database_mysql_backend MODULE ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_WRAPPED_CPP})
target_link_libraries(database_mysql_backend sql_backend_base ${Boost_LIBRARIES})
qt5_use_modules(database_mysql_backend Core Sql)
turnOnCpp11(database_mysql_backend)
hideSymbols(database_mysql_backend)
generate_export_header(database_mysql_backend EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_mysql_backend_export.h)

install(TARGETS database_mysql_backend RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
