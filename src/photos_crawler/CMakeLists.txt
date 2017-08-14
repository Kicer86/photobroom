
find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Threads)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ANALYZER_SOURCES
    default_analyzers/file_analyzer.cpp
    default_analyzers/file_extension_analyzer.cpp
    default_filesystem_scanners/filesystemscanner.cpp
    implementation/ifile_system_scanner.cpp
    implementation/photo_crawler.cpp
    implementation/photo_crawler_builder.cpp
    )

set(ANALYZER_HEADERS
    default_analyzers/file_analyzer.hpp
    default_analyzers/file_extension_analyzer.hpp
    default_filesystem_scanners/filesystemscanner.hpp
    ianalyzer.hpp
    ifile_system_scanner.hpp
    iphoto_crawler.hpp
    photo_crawler.hpp
    photo_crawler_builder.hpp
    )

source_group(photos_crawler REGULAR_EXPRESSION .*photos_crawler.* )

add_library(photos_crawler ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})
target_link_libraries(photos_crawler PRIVATE Qt5::Core ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(photos_crawler PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(photos_crawler)
hideSymbols(photos_crawler)

if(BUILD_SHARED_LIBS)
    install(TARGETS photos_crawler RUNTIME DESTINATION ${PATH_LIBS}
                                   LIBRARY DESTINATION ${PATH_LIBS})
endif()

if(BUILD_TESTS)
    include(photos_crawler_test.cmake)
endif()
