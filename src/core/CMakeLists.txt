
project(core)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Exiv2 REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${EXIV2_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/types.cpp
    implementation/tag_feeder.cpp
    )

set(CORE_HEADERS
    types.hpp
    tag_feeder.hpp
    )

if(BUILD_TESTS)
    find_package(GTest)
    include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
    set(CORE_SOURCES
        ${CORE_SOURCES}
        tests/coreTests.cpp
        tests/algoTests.cpp
       )
endif(BUILD_TESTS)


source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core SHARED ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core ${EXIV2_LIBRARIES})
qt5_use_modules(core Core)

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)

if(BUILD_TESTS)
    enableCodeCoverage(core)
    enableGTest(core GTEST)
endif(BUILD_TESTS)
