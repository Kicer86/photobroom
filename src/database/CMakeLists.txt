
project(database)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 REQUIRED COMPONENTS system filesystem)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(DATABASE_SOURCES
    implementation/backend.cpp
    implementation/databasebuilder.cpp
    implementation/entry.cpp
    implementation/memorydatabase.cpp
    )

set(DATABASE_HEADERS
    databasebuilder.hpp
    idatabase.hpp
    ifs.hpp
    entry.hpp
    implementation/backend.hpp
    implementation/memorydatabase.hpp
    )

if(BUILD_TESTS)

    addTestTarget(database  SOURCES
                                ${DATABASE_SOURCES}
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
