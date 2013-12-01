
project(analyzer)

add_definitions(-DBOOST_ALL_NO_LIB)                         #prevent boost from autolinking
find_package(Boost 1.53.0 COMPONENTS filesystem system regex)
find_package(OpenLibrary REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

set(ANALYZER_SOURCES
    default_analyzers/ext_defaultanalyzer.cpp
    implementation/file_analyzer.cpp
    implementation/filesystemscanner.cpp
    implementation/ianalyzer.cpp
    implementation/ifile_system_scanner.cpp
    implementation/iphoto_crawler.cpp
    implementation/photo_crawler.cpp
    implementation/photo_crawler_builder.cpp
    )

set(ANALYZER_HEADERS 
    default_analyzers/ext_defaultanalyzer.hpp 
    implementation/file_analyzer.hpp
    implementation/filesystemscanner.hpp
    implementation/ifile_system_scanner.hpp
    implementation/photo_crawler.hpp
    ianalyzer.hpp
    iphoto_crawler.hpp
    photo_crawler_builder.hpp
    )
    
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    find_package(GMock REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIR})
    set(ANALYZER_SOURCES ${ANALYZER_SOURCES}
        tests/analyzerTests.cpp
        tests/photo_crawler_tests.cpp
        ${GTEST_SOURCES}
        ${GMOCK_SOURCES}
       )
endif(BUILD_TESTS)

source_group(analyzer REGULAR_EXPRESSION .*analyzer.* )

add_library(analyzer SHARED ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})
target_link_libraries(analyzer ${Boost_LIBRARIES})

generate_export_header(analyzer EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/analyzer_export.h)
turnOnCpp11(analyzer)

if(BUILD_TESTS)
    enableTestsAndCodeCoverage(analyzer)
endif(BUILD_TESTS)
