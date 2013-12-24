
project(core)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Exiv2)

if(EXIV2)
	add_definitions(-DUSE_EXIV2=)
endif(EXIV2)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})

if(EXIV2)
    include_directories(SYSTEM ${EXIV2_INCLUDE_DIRS})
endif(EXIV2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/photo_info.cpp
    implementation/tag.cpp
    implementation/tag_feeder.cpp
    )

set(CORE_HEADERS
    tag.hpp
    tag_feeder.hpp
    photo_info.hpp
    )

if(BUILD_TESTS)
    addTestTarget(core
                        SOURCES
                            ${CORE_SOURCES}
                            tests/coreTests.cpp
                            tests/algoTests.cpp
                        TEST_LIBRARY
                            GTEST
                )
    turnOnCpp11(core_test)
    qt5_use_modules(core_test Core)
endif(BUILD_TESTS)


source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core SHARED ${CORE_SOURCES} ${CORE_HEADERS})
qt5_use_modules(core Core)

if(EXIV2)
	target_link_libraries(core ${EXIV2_LIBRARIES})
endif(EXIV2)

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)
