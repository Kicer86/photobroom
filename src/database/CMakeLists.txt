
add_subdirectory(backends)

if(BUILD_LEARNING_TESTS)
    add_subdirectory(learning_tests)
endif()

find_package(Qt6       REQUIRED COMPONENTS Core Gui)
find_package(Threads   REQUIRED)
find_package(MagicEnum REQUIRED)
find_package(OpenCV    REQUIRED)

set(DATABASE_SOURCES
    actions.hpp
    apeople_information_accessor.hpp
    aphoto_change_log_operator.hpp
    database_builder.hpp
    database_executor_traits.hpp
    database_queue.hpp
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
    iphoto_operator.hpp
    itransaction.hpp
    notifications_accumulator.hpp
    observable_database.hpp
    person_data.hpp
    photo_data.hpp
    photo_data_fields.hpp
    photo_types.hpp
    photo_utils.hpp
    project_info.hpp
    transaction_wrapper.hpp

    implementation/apeople_information_accessor.cpp
    implementation/aphoto_change_log_operator.cpp
    implementation/async_database.cpp
    implementation/async_database.hpp
    implementation/database_builder.cpp
    implementation/database_queue.cpp
    implementation/filter.cpp
    implementation/notifications_accumulator.cpp
    implementation/person_data.cpp
    implementation/photo_data.cpp
    implementation/photo_utils.cpp

    database_tools/common_backend_operations.hpp
    database_tools/id_to_data_converter.hpp
    database_tools/itag_info_collector.hpp
    database_tools/json_to_backend.hpp
    database_tools/photos_analyzer.hpp
    database_tools/series_candidate.hpp
    database_tools/series_detector.hpp
    database_tools/tag_info_collector.hpp

    database_tools/implementation/id_to_data_converter.cpp
    database_tools/implementation/json_to_backend.cpp
    database_tools/implementation/photos_analyzer.cpp
    database_tools/implementation/series_detector.cpp
    database_tools/implementation/tag_info_collector.cpp
)

add_library(database SHARED ${DATABASE_SOURCES})
target_link_libraries(database
                        PUBLIC
                            Qt::Gui
                        PRIVATE
                            core
                            Qt::Core
                            opencv_img_hash                 # do not link all possible libs, just those we will need (opencv_cvv uses Qt5 which causes problems)
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(database
                            PUBLIC
                                ${MAGIC_ENUM_INCLUDE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${OpenCV_INCLUDE_DIRS}
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
