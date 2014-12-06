
find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports .)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})

set(CONFIG_SOURCES
    private/configuration.cpp
    private/constants.cpp
    private/entrydata.cpp
    private/default_configuration_private.cpp
   )

set(CONFIG_HEADERS
    configuration.hpp
    constants.hpp
    entrydata.hpp
    iconfiguration.hpp
    private/default_configuration_private.hpp
   )


if(BUILD_TESTS)
    addTestTarget(configuration SOURCES
                                    ${CONFIG_SOURCES}
                                    tests/entrydata_tests.cpp
                                    tests/default_configuration_tests.cpp
                                    tests/default_configuration_private_tests.cpp
                                TEST_LIBRARY
                                    GTEST
       )
    target_link_libraries(configuration_test PRIVATE system Qt5::Core)
    turnOnCpp11(configuration_test)

endif(BUILD_TESTS)

source_group(configuration REGULAR_EXPRESSION .*configuration.* )

qt5_add_resources(CONFIG_RESOURCES ${CONFIG_RESOURCE_FILES})

add_library(configuration SHARED ${CONFIG_SOURCES} ${CONFIG_HEADERS})
target_link_libraries(configuration PRIVATE system Qt5::Core)

generate_export_header(configuration EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/configuration_export.h)
turnOnCpp11(configuration)
hideSymbols(configuration)

install(TARGETS configuration RUNTIME DESTINATION ${PATH_LIBS}
                              LIBRARY DESTINATION ${PATH_LIBS})
