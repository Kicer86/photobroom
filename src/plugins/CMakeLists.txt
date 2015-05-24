
find_package(Qt5Core REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports
                    ${CMAKE_SOURCE_DIR}/src
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    )

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS
    implementation/plugin_loader.cpp
)

set(HEADERS
    iplugin_loader.hpp
    plugin_loader.hpp
)

add_library(plugins OBJECT ${SRCS} ${HEADERS})
set_target_properties(plugins PROPERTIES POSITION_INDEPENDENT_CODE ON)

generate_export_header(plugins)
hideSymbols(plugins)
