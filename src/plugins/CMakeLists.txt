
find_package(OpenLibrary 2.0 REQUIRED putils)
find_package(Qt5Core REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS
    implementation/plugin_loader.cpp
)

set(HEADERS
    iplugin_loader.hpp
    plugin_loader.hpp
)

add_library(plugins STATIC ${SRCS} ${HEADERS})
target_link_libraries(plugins PRIVATE core database system Qt5::Core)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(plugins)
hideSymbols(plugins)
