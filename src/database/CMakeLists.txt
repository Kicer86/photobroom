
add_subdirectory(backends)

find_package(OpenLibrary 2.1 REQUIRED)
find_package(Qt6     REQUIRED COMPONENTS Core Gui)
find_package(Threads REQUIRED)
find_package(MagicEnum REQUIRED)

set(DATABASE_SOURCES
    actions.hpp
    apeople_information_accessor.hpp
    aphoto_change_log_operator.hpp
    database_builder.hpp
    database_executor_traits.hpp
    database_status.hpp
    filter.hpp
    general_flags.hpp
    group.hpp
    ibackend.hpp
    idatabase_builder.hpp
    idatabase.hpp
    idatabase_plugin.hpp
    igroup_operator.hpp
    ipeople_information_accessor.hpp
    iphoto_change_log_operator.hpp
    iphoto_info_cache.hpp
    iphoto_info.hpp
    iphoto_operator.hpp
    itransaction.hpp
    notifications_accumulator.hpp
    observable_database.hpp
    person_data.hpp
    photo_data.hpp
    photo_types.hpp
    photo_utils.hpp
    project_info.hpp

    implementation/apeople_information_accessor.cpp
    implementation/aphoto_change_log_operator.cpp
    implementation/async_database.cpp
    implementation/async_database.hpp
    implementation/database_builder.cpp
    implementation/filter.cpp
    implementation/notifications_accumulator.cpp
    implementation/person_data.cpp
    implementation/photo_data.cpp
    implementation/photo_info_cache.cpp
    implementation/photo_info_cache.hpp
    implementation/photo_info.cpp
    implementation/photo_info.hpp
    implementation/photo_utils.cpp

    database_tools/itag_info_collector.hpp
    database_tools/json_to_backend.hpp
    database_tools/photos_analyzer.hpp
    database_tools/photos_data_guesser.hpp
    database_tools/series_candidate.hpp
    database_tools/series_model.hpp
    database_tools/series_detector.hpp
    database_tools/signal_mapper.hpp
    database_tools/tag_info_collector.hpp

    database_tools/implementation/data_from_path_extractor.cpp
    database_tools/implementation/data_from_path_extractor.hpp
    database_tools/implementation/json_to_backend.cpp
    database_tools/implementation/photo_info_updater.cpp
    database_tools/implementation/photo_info_updater.hpp
    database_tools/implementation/photos_analyzer.cpp
    database_tools/implementation/photos_data_guesser.cpp
    database_tools/implementation/series_detector.cpp
    database_tools/implementation/series_model.cpp
    database_tools/implementation/signal_mapper.cpp
    database_tools/implementation/tag_info_collector.cpp
)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES})
target_link_libraries(database
                        PUBLIC
                            Qt::Gui
                        PRIVATE
                            core
                            Qt::Core
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(database
                            PRIVATE
                                ${OPENLIBRARY_INCLUDE_DIRS}
                                ${MAGIC_ENUM_INCLUDE_DIRS}
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                                ${OPENLIBRARY_INCLUDE_DIRS}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(database PROPERTIES AUTOMOC TRUE)

generate_export_header(database)
hideSymbols(database)

install(TARGETS database RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})

if(BUILD_TESTING)
    find_package(GTest REQUIRED CONFIG)
    include(database_tests.cmake)
    include(database_backends_tests.cmake)
endif()
