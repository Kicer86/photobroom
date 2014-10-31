
# openSSL for Windows: http://slproweb.com/products/Win32OpenSSL.html

find_package(OpenLibrary 2.0 REQUIRED putils)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenMP)
find_exif_package()
find_cryptographic_package()       # OpenSSL or Nettle

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDE_DIRS}
                           ${CRYPTOGRAPHIC_INCLUDE_DIR}
                           ${EXIF_INCLUDE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/atagfeeder.cpp
    implementation/base_tags.cpp
    implementation/hash_functions.cpp
    implementation/logger.cpp
    implementation/plugin_loader.cpp
    implementation/photos_manager.cpp
    implementation/tag.cpp
    implementation/tag_feeder_factory.cpp
    implementation/tag_updater.cpp
    implementation/task_executor.cpp
    implementation/time_guardian.cpp
    )

set(CORE_HEADERS
    base_tags.hpp
    hash_functions.hpp
    ilogger.hpp
    itagfeeder.hpp
    itask_executor.hpp
    iplugin_loader.hpp
    logger.hpp
    plugin_loader.hpp
    photos_manager.hpp
    tag.hpp
    tag_feeder_factory.hpp
    tag_updater.hpp
    task_executor.hpp
    time_guardian.hpp
    implementation/atagfeeder.hpp
    )

set(CORE_RESOURCE_FILES images/images.qrc)

qt5_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})


if(EXIV2_FOUND)

    add_definitions(-DUSE_EXIV2)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/exiv2_tag_feeder.cpp)
    set(CORE_HEADERS ${CORE_HEADERS} implementation/exiv2_tag_feeder.hpp)

elseif(EASYEXIF_FOUND)

    add_definitions(-DUSE_EASY_EXIF)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/easy_exif_tag_feeder.cpp)
    set(CORE_HEADERS ${CORE_HEADERS} implementation/easy_exif_tag_feeder.hpp)

endif(EXIV2_FOUND)


if(OPENSSL_FOUND)
    add_definitions(-DUSE_OPENSSL)
elseif(NETTLE_FOUND)
    add_definitions(-DUSE_NETTLE)
endif(OPENSSL_FOUND)


if(BUILD_TESTS)
    addTestTarget(core
                        SOURCES
                            ${CORE_SOURCES}
                            ${CORE_RESOURCES}
                            tests/coreTests.cpp
                            tests/algoTests.cpp
                        TEST_LIBRARY
                            GTEST
                )
    turnOnCpp11(core_test)
    target_link_libraries(core_test ${OPENSSL_LIBRARIES} ${OPENLIBRARY_LIBRARIES} ${EXIF_LIBRARIES} Qt5::Core Qt5::Gui)
    if(MSVC)

    else()
        target_link_libraries(core_test ${OpenMP_CXX_FLAGS})
    endif(MSVC)

endif(BUILD_TESTS)

#task executor uses openmp
if(OPENMP_FOUND)
    addSourceFlags(implementation/task_executor.cpp COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    add_definitions(-DUSE_OPENMP)
endif()

#addSourceFlags(implementation/photo_info.cpp COMPILE_FLAGS -fgnu-tm)

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES} ${CORE_HEADERS})
target_link_libraries(core PRIVATE system ${OPENLIBRARY_LIBRARIES} Qt5::Core Qt5::Gui ${CRYPTOGRAPHIC_LIBRARIES} ${EXIF_LIBRARIES})

if(OPENMP_FOUND)
    if(MSVC)

    else()
        target_link_libraries(core PRIVATE ${OpenMP_CXX_FLAGS})
    endif(MSVC)
endif(OPENMP_FOUND)


generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)
hideSymbols(core)

install(TARGETS core DESTINATION ${PATH_LIBS})
