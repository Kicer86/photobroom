
project(database)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 COMPONENTS crc integer)

include(GenerateExportHeader)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/exports ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)

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
    find_package(GTest)
    include_directories(${GTEST_INCLUDE_DIRS})
    set(DATABASE_SOURCES 
        ${DATABASE_SOURCES} 
        tests/databaseTests.cpp
       )
endif(BUILD_TESTS)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS})

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)

if(BUILD_TESTS)
    enableCodeCoverage(database)    
    enableGTest(database GTEST)    
endif(BUILD_TESTS)
