
project(analyzer)

find_package(OpenLibrary REQUIRED)
include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} .)

set(ANALYZER_SOURCES
    implementation/filesystemscanner.cpp
	implementation/photo_crawler.cpp
	)

set(ANALYZER_HEADERS 
    implementation/filesystemscanner.hpp
    implementation/ifile_system_scanner.hpp
    implementation/photo_crawler.hpp
	iphoto_crawler.hpp)

source_group(analyzer REGULAR_EXPRESSION .*analyzer.* )

add_library(analyzer SHARED ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})

exportSymbols(analyzer)
hideSymbols(analyzer)
turnOnCpp11(analyzer)
