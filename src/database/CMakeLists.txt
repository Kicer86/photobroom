
add_subdirectory(backends)

find_package(OpenLibrary 2.1 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Threads REQUIRED)

set(DATABASE_SOURCES
    implementation/async_database.cpp
    implementation/database_builder.cpp
    implementation/filter.cpp
    implementation/group.cpp
    implementation/person_data.cpp
    implementation/photo_data.cpp
    implementation/photo_info.cpp
    implementation/photo_info_cache.cpp
    implementation/photo_types.cpp
    database_tools/implementation/photos_analyzer.cpp
    database_tools/implementation/photo_info_updater.cpp
    database_tools/implementation/photo_duplicates_finder.cpp
    database_tools/implementation/synchronous_database.cpp
    database_tools/implementation/tag_info_collector.cpp
    database_tools/implementation/signal_mapper.cpp
    database_tools/implementation/series_detector.cpp
)

set(DATABASE_HEADERS
    database_builder.hpp
    database_status.hpp
    filter.hpp
    group.hpp
    ibackend.hpp
    idatabase.hpp
    idatabase_builder.hpp
    idatabase_plugin.hpp
    igroup_operator.hpp
    iphoto_change_log_operator.hpp
    iphoto_info.hpp
    iphoto_info_cache.hpp
    iphoto_operator.hpp
    person_data.hpp
    photo_data.hpp
    photo_iterator.hpp
    photo_types.hpp
    query_list.hpp
    project_info.hpp
    implementation/async_database.hpp
    implementation/photo_info.hpp
    implementation/photo_info_cache.hpp
    database_tools/photos_analyzer.hpp
    database_tools/tag_info_collector.hpp
    database_tools/signal_mapper.hpp
    database_tools/implementation/photo_info_updater.hpp
    database_tools/implementation/photo_duplicates_finder.hpp
    database_tools/implementation/synchronous_database.hpp
    database_tools/series_detector.hpp
)

set(DATABASE_MOC
    ibackend.hpp
    idatabase_plugin.hpp
    database_tools/itag_info_collector.hpp
    database_tools/photos_analyzer.hpp
    database_tools/implementation/photos_analyzer_p.hpp
    database_tools/signal_mapper.hpp
)

qt5_wrap_cpp(DATABASE_MOCED ${DATABASE_MOC})

stringify_enums(STRINGIFIED_ENUMS database_status.hpp)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS} ${DATABASE_MOCED} ${STRINGIFIED_ENUMS})
target_link_libraries(database
                        PUBLIC
                            Qt5::Gui
                        PRIVATE
                            core
                            Qt5::Core
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(database
                            SYSTEM PRIVATE
                                ${OPENLIBRARY_INCLUDE_DIRS}
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                                ${OPENLIBRARY_INCLUDE_DIRS}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_CURRENT_SOURCE_DIR}
)

generate_export_header(database)
hideSymbols(database)

install(TARGETS database RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})

if(BUILD_TESTS)
    include(database_tests.cmake)
    include(database_integration_tests.cmake)
endif()
