
project(SQLite_Backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 REQUIRED filesystem system)

add_definitions(-DBOOST_ALL_NO_LIB)                         #prevent boost from autolinking
include_directories(${CMAKE_SOURCE_DIR}/src/
                    ${CMAKE_SOURCE_DIR}/src/database/
                    ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(BACKEND_SOURCES backend.cpp)
set(BACKEND_HEADERS backend.hpp)

qt5_wrap_cpp(BACKEND_MOCS ${BACKEND_HEADERS})

add_library(database_sqlite_backend MODULE ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_MOCS})
target_link_libraries(database_sqlite_backend sql_backend_base ${Boost_LIBRARIES})
qt5_use_modules(database_sqlite_backend Core Sql)
turnOnCpp11(database_sqlite_backend)
hideSymbols(database_sqlite_backend)
generate_export_header(database_sqlite_backend EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_sqlite_backend_export.h)

install(TARGETS database_sqlite_backend RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
