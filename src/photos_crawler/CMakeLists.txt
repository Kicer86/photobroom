
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Threads)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports)

set(ANALYZER_SOURCES
    default_analyzers/file_analyzer.cpp
    default_filesystem_scanners/filesystemscanner.cpp
    implementation/ifile_system_scanner.cpp
    implementation/photo_crawler.cpp
    implementation/photo_crawler_builder.cpp
)

set(ANALYZER_HEADERS
    default_analyzers/file_analyzer.hpp
    default_filesystem_scanners/filesystemscanner.hpp
    ianalyzer.hpp
    ifile_system_scanner.hpp
    iphoto_crawler.hpp
    photo_crawler.hpp
    photo_crawler_builder.hpp
)

source_group(photos_crawler REGULAR_EXPRESSION .*photos_crawler.* )

add_library(photos_crawler ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})

target_link_libraries(photos_crawler
                        PRIVATE
                            core
                            Qt::Core
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(photos_crawler
                                PUBLIC
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
