
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)


addTestTarget(photos_crawler
                SOURCES
                    default_analyzers/file_analyzer.cpp
                    default_analyzers/file_extension_analyzer.cpp
                    implementation/ianalyzer.cpp
                    implementation/ifile_system_scanner.cpp
                    implementation/photo_crawler.cpp

                    unit_tests/analyzerTests.cpp
                    unit_tests/photo_crawler_tests.cpp
                    unit_tests/photo_crawler_builder_tests.cpp

                LIBRARIES
                    PRIVATE
                    Qt5::Core
                    ${GMOCK_MAIN_LIBRARY}
                    ${GMOCK_LIBRARY}
                    ${GTEST_LIBRARY}
                    ${CMAKE_THREAD_LIBS_INIT}

                INCLUDES
                    SYSTEM PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
)
