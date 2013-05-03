
project(analyzer)

add_definitions(-DBOOST_ALL_NO_LIB)                         #prevent boost from autolinking
find_package(Boost 1.53.0 COMPONENTS filesystem system)
find_package(OpenLibrary REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

set(ANALYZER_SOURCES
	implementation/file_analyzer.cpp
    implementation/filesystemscanner.cpp
	implementation/photo_crawler.cpp
	)

set(ANALYZER_HEADERS 
	implementation/file_analyzer.hpp
    implementation/filesystemscanner.hpp
    implementation/ifile_system_scanner.hpp
	implementation/ifile_analyzer.hpp
    implementation/photo_crawler.hpp
	iphoto_crawler.hpp)

source_group(analyzer REGULAR_EXPRESSION .*analyzer.* )

add_library(analyzer SHARED ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})
target_link_libraries(analyzer ${Boost_LIBRARIES})

exportSymbols(analyzer)
hideSymbols(analyzer)
turnOnCpp11(analyzer)
