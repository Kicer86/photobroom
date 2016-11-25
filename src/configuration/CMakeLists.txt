
find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(JsonCpp REQUIRED)

include(CheckTypeSize)
set(CMAKE_REQUIRED_INCLUDES   ${Qt5Core_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})
set(CMAKE_EXTRA_INCLUDE_FILES QtGlobal json/config.h)
set(CMAKE_REQUIRED_LIBRARIES  Qt5::Core)

check_type_size("qlonglong"   QLONGLONG  LANGUAGE CXX)
check_type_size("Json::Int64" JSON_INT64 LANGUAGE CXX)

if (NOT QLONGLONG EQUAL JSON_INT64)
    message(FATAL_ERROR "qlonglong and Json::Int64 do not have same size")
endif()

# JsonCpp is used instead of QJson* classes die to problem with nested QJsonObjects:
# http://stackoverflow.com/questions/17034336/how-to-change-qjsonobject-value-in-a-qjson-hierarchy-without-using-copies

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

set(CONFIG_SOURCES
    private/configuration.cpp
    private/constants.cpp
   )

set(CONFIG_HEADERS
    configuration.hpp
    constants.hpp
    iconfiguration.hpp
    private/configuration_p.hpp
   )


source_group(configuration REGULAR_EXPRESSION .*configuration.* )

qt5_add_resources(CONFIG_RESOURCES ${CONFIG_RESOURCE_FILES})

add_library(configuration ${CONFIG_SOURCES} ${CONFIG_HEADERS})

target_link_libraries(configuration
                        PRIVATE
                            core
                            system
                            Qt5::Core
                            ${JSONCPP_LIBRARIES}
)

target_include_directories(configuration
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}

                                SYSTEM PRIVATE
                                    ${OPENLIBRARY_INCLUDE_DIRS}
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${JSONCPP_INCLUDE_DIRS}

                                PRIVATE
                                    ${CMAKE_CURRENT_SOURCE_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
)

generate_export_header(configuration)
hideSymbols(configuration)

if(BUILD_SHARED_LIBS)
    install(TARGETS configuration RUNTIME DESTINATION ${PATH_LIBS}
                                  LIBRARY DESTINATION ${PATH_LIBS})
endif()
