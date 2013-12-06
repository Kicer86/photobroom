
project(configuration)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(GTest)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${Qt5Core_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${CMAKE_BINARY_DIR}/exports .)

set(CONFIG_SOURCES
    private/configurationfactory.cpp
    private/entrydata.cpp
    private/default_configuration.cpp
   )

set(CONFIG_HEADERS
    configurationfactory.hpp
    entrydata.hpp
    iconfiguration.hpp
    private/default_configuration.hpp
   )
   
if(BUILD_TESTS)
    include_directories(${GTEST_INCLUDE_DIRS})
    set(CONFIG_SOURCES 
        ${CONFIG_SOURCES} 
        tests/entrydata_tests.cpp
        tests/configurationfactory_tests.cpp
        tests/configurationfactory_tests.cpp
       )
endif(BUILD_TESTS)

source_group(configuration REGULAR_EXPRESSION .*configuration.* )

add_library(configuration SHARED ${CONFIG_SOURCES} ${CONFIG_HEADERS})
qt5_use_modules(configuration Core)

generate_export_header(configuration EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/configuration_export.h)
turnOnCpp11(configuration)

if(BUILD_TESTS)
    enableCodeCoverage(configuration)    
    enableGTest(configuration GTEST)    
endif(BUILD_TESTS)
