
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary REQUIRED utils)

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

set(PROJECT_UTILS_MOC )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_SOURCE_DIR}/src
                    $<TARGET_PROPERTY:plugins,INTERFACE_INCLUDE_DIRECTORIES>
)

include_directories(SYSTEM
                    ${Qt5Core_INCLUDE_DIRS}
                    ${OPENLIBRARY_INCLUDE_DIRS}
)

qt5_wrap_cpp(PROJECT_UTILS_MOCED ${PROJECT_UTILS_MOC})

add_library(project_utils ${PROJECT_UTILS_SOURCES} ${PROJECT_UTILS_HEADERS} ${PROJECT_UTILS_MOCED})
target_link_libraries(project_utils PRIVATE
                                    core
                                    database
                                    system
                                    Qt5::Core
                                    Qt5::Gui
                                    ${OPENLIBRARY_LIBRARIES}
                     )
target_include_directories(project_utils PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(project_utils)
hideSymbols(project_utils)

if(BUILD_SHARED_LIBS)
    install(TARGETS project_utils RUNTIME DESTINATION ${PATH_LIBS}
                                  LIBRARY DESTINATION ${PATH_LIBS})
endif()
