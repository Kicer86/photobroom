
find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

    addTestTarget(analyzer
                            TEST_LIBRARY
                                GMOCK
                            SOURCES
                                ${ANALYZER_SOURCES}
                                tests/analyzerTests.cpp
                                tests/photo_crawler_tests.cpp
                                tests/photo_crawler_builder_tests.cpp
                )
    target_link_libraries(analyzer_test PRIVATE Qt5::Core)
    turnOnCpp11(analyzer_test)

endif(BUILD_TESTS)

source_group(analyzer REGULAR_EXPRESSION .*analyzer.* )

add_library(analyzer SHARED ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})
target_link_libraries(analyzer PRIVATE Qt5::Core)

generate_export_header(analyzer EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/analyzer_export.h)
turnOnCpp11(analyzer)
hideSymbols(analyzer)

install(TARGETS analyzer RUNTIME DESTINATION ${PATH_LIBS}
		         LIBRARY DESTINATION ${PATH_LIBS})
