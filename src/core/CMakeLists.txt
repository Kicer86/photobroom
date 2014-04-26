
project(core CXX)

# openSSL for Windows: http://slproweb.com/products/Win32OpenSSL.html

find_package(OpenLibrary REQUIRED palgorithm)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenMP)
find_package(Exiv2)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDES}
                           ${OPENSSL_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/aphoto_info.cpp
    implementation/base_tags.cpp
    implementation/hash_functions.cpp
    implementation/itagfeeder.cpp
    implementation/tag.cpp
    implementation/task_executor.cpp
    implementation/time_guardian.cpp
    )

set(CORE_HEADERS
    aphoto_info.hpp
    base_tags.hpp
    hash_functions.hpp
    itagfeeder.hpp
    tag.hpp
    task_executor.hpp
    time_guardian.hpp
    )

set(CORE_MOC
    aphoto_info.hpp)

qt5_wrap_cpp(CORE_MOCED ${CORE_MOC})

if(EXIV2_FOUND)
    add_definitions(-DUSE_EXIV)
    include_directories(SYSTEM ${EXIV2_INCLUDE_DIRS})
    set(CORE_SOURCES ${CORE_SOURCES} implementation/exif_tag_feeder.cpp)
    set(CORE_HEADERS ${CORE_HEADERS} implementation/exif_tag_feeder.hpp)
endif(EXIV2_FOUND)


if(BUILD_TESTS)
    addTestTarget(core
                        SOURCES
                            ${CORE_SOURCES}
                            ${CORE_MOCED}
                            tests/coreTests.cpp
                            tests/algoTests.cpp
                        TEST_LIBRARY
                            GTEST
                )
    turnOnCpp11(core_test)
    qt5_use_modules(core_test Core Gui)
    target_link_libraries(core_test ${OPENSSL_LIBRARIES} ${OPENLIBRARY_PALGORITHM_LIBRARIES} ${OpenMP_CXX_FLAGS})

    #TODO: remove it?
    if(EXIV2_FOUND)
        target_link_libraries(core_test ${EXIV2_LIBRARIES})
    endif(EXIV2_FOUND)

endif(BUILD_TESTS)

#task executor uses openmp
if(OPENMP_FOUND)
    addSourceFlags(implementation/task_executor.cpp COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    add_definitions(-DUSE_OPENMP)
endif()

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_HEADERS} ${CORE_MOCED})
target_link_libraries(core ${OPENSSL_LIBRARIES} ${OPENLIBRARY_PALGORITHM_LIBRARIES} ${OpenMP_CXX_FLAGS})
qt5_use_modules(core Core Gui)
addFlags(core LINK_FLAGS ${OpenMP_CXX_FLAGS})

if(EXIV2_FOUND)
    target_link_libraries(core ${EXIV2_LIBRARIES})
endif(EXIV2_FOUND)

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)
hideSymbols(core)

install(TARGETS core RUNTIME LIBRARY DESTINATION ${PATH_LIBS})
