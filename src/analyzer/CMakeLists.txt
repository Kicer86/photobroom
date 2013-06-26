
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
	photo_crawler_builder.hpp)

source_group(analyzer REGULAR_EXPRESSION .*analyzer.* )

add_library(analyzer SHARED ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})
target_link_libraries(analyzer ${Boost_LIBRARIES})

generate_export_header(analyzer EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/analyzer_export.h)
turnOnCpp11(analyzer)
turnOnAllWarnings(${ANALYZER_SOURCES})


#############################################################

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(CppUTest REQUIRED)

include_directories(${CPPUTEST_INCLUDE_DIRS})

set(TESTS
    tests/analyzerTests.cpp
	tests/RunAllTests.cpp 
	${ANALYZER_SOURCES}
    )

add_executable(analyzer_tests ${TESTS})
target_link_libraries(analyzer_tests ${CPPUTEST_LIBRARIES} ${Boost_LIBRARIES})
set_target_properties(analyzer_tests PROPERTIES COMPILE_FLAGS -DANALYZER_STATIC_DEFINE)

turnOnCpp11(analyzer_tests)
