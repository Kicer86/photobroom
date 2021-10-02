
find_package(Qt6 REQUIRED COMPONENTS Core Gui)

include(GenerateExportHeader)

set(SRCS
    implementation/iplugin_loader.cpp
    implementation/plugin_loader.cpp
)

set(HEADERS
    iplugin_loader.hpp
    plugin_loader.hpp
)

add_library(plugins OBJECT ${SRCS} ${HEADERS})
set_target_properties(plugins PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(plugins
                            PRIVATE
                                ${CMAKE_BINARY_DIR}/exports
                                ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_CURRENT_SOURCE_DIR}
                            PRIVATE
                                ${Qt6Core_INCLUDE_DIRS}
                                ${Qt6Gui_INCLUDE_DIRS}
)

target_link_libraries(plugins
                        PRIVATE
                            core
                            database
                            system
)

generate_export_header(plugins)
hideSymbols(plugins)
