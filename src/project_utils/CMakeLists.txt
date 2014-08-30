
project(project_utils CXX)

set(PROJECT_UTILS_SOURCES
    implementation/project.cpp
    implementation/project_manager.cpp
    )

set(PROJECT_UTILS_HEADERS
    iproject.hpp
    iproject_manager.hpp
    project.hpp
    project_manager.hpp
   )

set(PROJECT_UTILS_MOC )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
#include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDE_DIRS})

qt5_wrap_cpp(PROJECT_UTILS_MOCED ${PROJECT_UTILS_MOC})

add_library(project_utils ${PROJECT_UTILS_SOURCES} ${PROJECT_UTILS_HEADERS} ${PROJECT_UTILS_MOCED})
#target_link_libraries(project_utils core)

#qt5_use_modules(project_utils Core Gui)

generate_export_header(project_utils EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/project_utils_export.h)
turnOnCpp11(project_utils)
hideSymbols(project_utils)

install(TARGETS project_utils DESTINATION ${PATH_LIBS})
