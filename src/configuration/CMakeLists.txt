
project(configuration)

find_package(OpenLibrary REQUIRED)
find_package(Boost REQUIRED)
find_package(Qt5Core REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports .)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})

set(CONFIG_SOURCES
    private/configurationfactory.cpp
    private/constraints.cpp
    private/entrydata.cpp
    private/default_configuration.cpp
    private/default_configuration_private.cpp
   )

set(CONFIG_HEADERS
    configurationfactory.hpp
    constraints.hpp
    entrydata.hpp
    iconfiguration.hpp
    private/default_configuration.hpp
    private/default_configuration_private.hpp
   )

set(CONFIG_RESOURCE_FILES rc/resources.qrc)

if(BUILD_TESTS)
    addTestTarget(configuration SOURCES
                                    ${CONFIG_SOURCES}
                                    tests/entrydata_tests.cpp
                                    tests/configurationfactory_tests.cpp
                                    tests/default_configuration_tests.cpp
                                    tests/default_configuration_private_tests.cpp
                                TEST_LIBRARY
                                    GTEST
       )
    target_link_libraries(configuration_test system)
    turnOnCpp11(configuration_test)
    qt5_use_modules(configuration_test Core)

endif(BUILD_TESTS)

source_group(configuration REGULAR_EXPRESSION .*configuration.* )

qt5_add_resources(CONFIG_RESOURCES ${CONFIG_RESOURCE_FILES})

add_library(configuration SHARED ${CONFIG_SOURCES} ${CONFIG_HEADERS} ${CONFIG_RESOURCES})
target_link_libraries(configuration system)
qt5_use_modules(configuration Core)

generate_export_header(configuration EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/configuration_export.h)
turnOnCpp11(configuration)
