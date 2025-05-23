
find_package(Qt6 REQUIRED COMPONENTS Core)

include(GenerateExportHeader)

set(ANALYZER_SOURCES
    default_analyzers/file_analyzer.cpp
    default_filesystem_scanners/filesystemscanner.cpp
    implementation/photo_crawler.cpp
)

set(ANALYZER_HEADERS
    default_analyzers/file_analyzer.hpp
    default_filesystem_scanners/filesystemscanner.hpp
    ianalyzer.hpp
    ifile_system_scanner.hpp
    iphoto_crawler.hpp
    photo_crawler.hpp
)

add_library(photos_crawler ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})

target_link_libraries(photos_crawler
                        PRIVATE
                            core
                            Qt::Core
)

target_include_directories(photos_crawler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..    # force clients to use scoped include: #include <photos_crawler/...>
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

generate_export_header(photos_crawler)
hideSymbols(photos_crawler)

if(BUILD_SHARED_LIBS)
    install(TARGETS photos_crawler RUNTIME DESTINATION ${PATH_LIBS}
                                   LIBRARY DESTINATION ${PATH_LIBS})
endif()

if(BUILD_TESTING)
    include(photos_crawler_test.cmake)
endif()
