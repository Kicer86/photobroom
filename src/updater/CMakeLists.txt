
add_subdirectory(github_api)

find_package(Qt5Network REQUIRED)

include(GenerateExportHeader)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${Qt5Network_INCLUDE_DIRS})


set(SRCS
    updater.cpp
    implementation/updater_impl.cpp
    implementation/version.cpp
    )

set(HEADERS
    updater.hpp
    implementation/updater_impl.hpp
    implementation/version.hpp
    )

#set of file to be moced
set(TO_MOC
    implementation/updater_impl.hpp
    )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(updater ${SRCS} ${MOCED} ${HEADERS})
target_link_libraries(updater PRIVATE github_api Qt5::Network)
target_include_directories(updater PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(updater)
hideSymbols(updater)

