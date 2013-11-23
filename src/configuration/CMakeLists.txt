
project(configuration)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${Qt5Core_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${CMAKE_BINARY_DIR}/exports .)

set(CONFIG_SOURCES
    configurationfactory.cpp
    entrydata.cpp
   )


set(CONFIG_HEADERS
    configurationfactory.hpp
    entrydata.hpp
    iconfiguration.hpp
   )

source_group(configuration REGULAR_EXPRESSION .*configuration.* )

add_library(configuration SHARED ${CONFIG_SOURCES} ${CONFIG_HEADERS})
qt5_use_modules(configuration Core)

generate_export_header(configuration EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/configuration_export.h)
turnOnCpp11(configuration)


##################################################################################


if(BUILD_TESTS)

    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

    find_package(CppUTest REQUIRED)

        include_directories(${CPPUTEST_INCLUDE_DIRS})

        set(TESTS
            tests/entrydata_tests.cpp
            tests/RunAllTests.cpp
            ${CONFIG_SOURCES}
        )

    add_executable(configuration_tests ${TESTS})
    qt5_use_modules(configuration_tests Core)
    set_target_properties(configuration_tests PROPERTIES COMPILE_FLAGS -DCONFIGURATION_STATIC_DEFINE)
    enableCodeCoverageForSources(configuration_tests ${CONFIG_SOURCES})
    target_link_libraries(configuration_tests ${CPPUTEST_LIBRARIES})

    turnOnCpp11(configuration_tests)
    
    add_test(configuration configuration_tests)

endif(BUILD_TESTS)
