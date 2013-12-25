
project(configuration)

find_package(OpenLibrary REQUIRED)
find_package(Boost REQUIRED)
find_package(Qt5Core REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports .)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})

if(UNIX)
    set(CONF_DIR "$ENV{HOME}/.config/broom")
elseif(WIN32)
    set(CONF_DIR "$ENV{APPDATA}/broom")
else()
    message(FATAL_ERROR "Problems with determining Operating System for configruation dir purposes")
endif(UNIX)

configure_file(rc/base_config.xml.template ${CMAKE_CURRENT_BINARY_DIR}/base_config.xml @ONLY)
configure_file(rc/resources.qrc.template   ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc   COPYONLY)

set(CONFIG_SOURCES
    private/configurationfactory.cpp
    private/entrydata.cpp
    private/default_configuration.cpp
    private/xml_reader.cpp
    private/xml_reader_builder.cpp
   )

set(CONFIG_HEADERS
    configurationfactory.hpp
    entrydata.hpp
    iconfiguration.hpp
    private/default_configuration.hpp
    private/ixml_reader.hpp
    private/xml_reader.hpp
    private/xml_reader_builder.hpp
   )

set(CONFIG_RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc)   #file needs to be configured

if(BUILD_TESTS)
    addTestTarget(configuration SOURCES
                                    ${CONFIG_SOURCES}
                                    tests/entrydata_tests.cpp
                                    tests/configurationfactory_tests.cpp
                                    tests/default_configuration_tests.cpp
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

