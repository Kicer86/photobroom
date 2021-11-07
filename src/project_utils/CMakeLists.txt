
find_package(Qt6 REQUIRED COMPONENTS Core Gui)

set(PROJECT_UTILS_SOURCES
    implementation/misc.cpp
    implementation/project.cpp
    implementation/project_manager.cpp
)

set(PROJECT_UTILS_HEADERS
    iproject_manager.hpp
    misc.hpp
    project.hpp
    project_manager.hpp
)

add_library(project_utils ${PROJECT_UTILS_SOURCES} ${PROJECT_UTILS_HEADERS})
target_link_libraries(project_utils PRIVATE
                                    core
                                    database
                                    system
                                    Qt::Core
                                    Qt::Gui
)

target_include_directories(project_utils
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

generate_export_header(project_utils)
hideSymbols(project_utils)

if(BUILD_SHARED_LIBS)
    install(TARGETS project_utils RUNTIME DESTINATION ${PATH_LIBS}
                                  LIBRARY DESTINATION ${PATH_LIBS})
endif()
