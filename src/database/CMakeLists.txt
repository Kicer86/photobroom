
project(database)

add_subdirectory(implementation/reusable)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 REQUIRED COMPONENTS system filesystem)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(DATABASE_SOURCES
    implementation/databasebuilder.cpp
    implementation/entry.cpp
    implementation/memorydatabase.cpp
    )

set(DATABASE_HEADERS
    databasebuilder.hpp
    idatabase.hpp
    ifs.hpp
    implementation/memorydatabase.hpp
    )

if(BUILD_TESTS)
    addTestTarget(database  SOURCES
                                implementation/entry.cpp
                                implementation/memorydatabase.cpp
                                tests/databaseTests.cpp
                            TEST_LIBRARY
                                GTEST
       )
    target_link_libraries(database_test configuration ${Boost_LIBRARIES})
    turnOnCpp11(database_test)
endif(BUILD_TESTS)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS})
target_link_libraries(database configuration ${Boost_LIBRARIES})

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)


set(BACKEND mysql)

#determine which backend should be used
if(BACKEND STREQUAL "mysql")

    add_subdirectory(implementation/mysql_backend)
    target_link_libraries(database database_mysql_backend)

endif(BACKEND STREQUAL "mysql")
