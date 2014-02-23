
project(core)

find_package(OpenLibrary REQUIRED palgorithm)
find_package(Qt5Core REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Exiv2)

if(EXIV2_FOUND)
	add_definitions(-DUSE_EXIV)
endif(EXIV2_FOUND)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES})

if(EXIV2_FOUND)
    include_directories(SYSTEM ${EXIV2_INCLUDE_DIRS})
endif(EXIV2_FOUND)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/aphoto_info.cpp
    implementation/base_tags.cpp
    implementation/hash_functions.cpp
    implementation/tag.cpp
    implementation/tag_feeder.cpp
    implementation/task_executor.cpp
    )

set(CORE_HEADERS
    aphoto_info.hpp
    base_tags.hpp
    hash_functions.hpp
    tag.hpp
    tag_feeder.hpp
    task_executor.hpp
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
    target_link_libraries(core_test ${OPENSSL_LIBRARIES} ${OPENLIBRARY_PALGORITHM_LIBRARIES} ${OpenMP_CXX_FLAGS})
endif(BUILD_TESTS)

#task executor uses openmp
addSourceFlags(implementation/task_executor.cpp COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core ${OPENSSL_LIBRARIES} ${OPENLIBRARY_PALGORITHM_LIBRARIES} ${OpenMP_CXX_FLAGS})
qt5_use_modules(core Core)
#addFlags(core LINK_FLAGS ${OpenMP_CXX_FLAGS})

if(EXIV2_FOUND)
    target_link_libraries(core ${EXIV2_LIBRARIES})
endif(EXIV2_FOUND)

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)

if(EXIV2_FOUND)
    message(STATUS "Exiv2 library found")
else()
    message(WARNING "Optional library Exiv2 not found")
endif(EXIV2_FOUND)
