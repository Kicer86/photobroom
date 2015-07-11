
add_subdirectory(backends)

find_package(OpenLibrary 2.1 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM
                    ${OPENLIBRARY_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(DATABASE_SOURCES
    implementation/database_builder.cpp
    implementation/database_thread.cpp
    implementation/filter.cpp
    implementation/photo_info.cpp
    implementation/photo_info_cache.cpp
    implementation/photo_info_storekeeper.cpp

    database_tools/implementation/photos_analyzer.cpp
    database_tools/implementation/photo_info_updater.cpp
    database_tools/implementation/photo_duplicates_finder.cpp
    database_tools/implementation/synchronous_database.cpp
    )

set(DATABASE_HEADERS
    database_builder.hpp
    database_status.hpp
    filter.hpp
    idatabase.hpp
    iphoto_info.hpp
    iphoto_info_manager.hpp
    ifs.hpp
    photo_iterator.hpp
    query_list.hpp
    project_info.hpp
    backends/photo_info.hpp
    implementation/database_thread.hpp
    implementation/photo_info_cache.hpp
    implementation/photo_info_storekeeper.hpp
    database_tools/photos_analyzer.hpp
    database_tools/implementation/photo_info_updater.hpp
    database_tools/implementation/photo_duplicates_finder.hpp
    database_tools/implementation/synchronous_database.hpp
    )

set(DATABASE_MOC 
    idatabase.hpp
    idatabase_plugin.hpp
    database_tools/photos_analyzer.hpp
    database_tools/implementation/photos_analyzer_p.hpp)

qt5_wrap_cpp(DATABASE_MOCED ${DATABASE_MOC})

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS} ${DATABASE_MOCED})
target_link_libraries(database PRIVATE configuration core Qt5::Core Qt5::Gui)

#define interface of database
target_include_directories(database INTERFACE ${CMAKE_CURRENT_BINARY_DIR} ${OPENLIBRARY_INCLUDE_DIRS})

generate_export_header(database)
hideSymbols(database)

install(TARGETS database RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})
