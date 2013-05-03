
project(analyzer)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 COMPONENTS filesystem)

include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)

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
target_link_libraries(analyzer ${Boost_LIBRARIES})

exportSymbols(analyzer)
hideSymbols(analyzer)
turnOnCpp11(analyzer)
