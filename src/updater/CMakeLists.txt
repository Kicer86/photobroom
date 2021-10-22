
set(GitHubAPI_QtBackend ON)
set(GitHubAPI_UseQt6 TRUE)
add_subdirectory(github_api)

find_package(Qt6 REQUIRED COMPONENTS Network)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(SRCS
    iupdater.cpp
    updater.cpp
    implementation/updater_impl.cpp
    implementation/version.cpp
)

set(HEADERS
    iupdater.hpp
    updater.hpp
    implementation/updater_impl.hpp
    implementation/version.hpp
)

#set of file to be moced
set(TO_MOC
    implementation/updater_impl.hpp
)

qt6_wrap_cpp(MOCED ${TO_MOC})

add_library(updater ${SRCS} ${MOCED} ${HEADERS})
target_link_libraries(updater PRIVATE github_api Qt::Network)
target_include_directories(updater PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(updater)
hideSymbols(updater)

if(BUILD_SHARED_LIBS)
    install(TARGETS updater RUNTIME DESTINATION ${PATH_LIBS}
                            LIBRARY DESTINATION ${PATH_LIBS})

    install(TARGETS github_api RUNTIME DESTINATION ${PATH_LIBS}
                               LIBRARY DESTINATION ${PATH_LIBS})
endif()


