
find_package(OpenLibrary 2.1 REQUIRED putils)
find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(Threads REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/aexif_reader.cpp
    implementation/base_tags.cpp
    implementation/cross_thread_callback.cpp
    implementation/disk_observer.cpp
    implementation/exif_reader_factory.cpp
    implementation/exiv2_exif_reader.cpp
    implementation/file_lock.cpp
    implementation/ilogger.cpp
    implementation/log_file_rotator.cpp
    implementation/logger.cpp
    implementation/logger_factory.cpp
    implementation/media_types.cpp
    implementation/photo_information.cpp
    implementation/search_expression_evaluator.cpp
    implementation/stopwatch.cpp
    implementation/tag.cpp
    implementation/task_executor.cpp
    implementation/time_guardian.cpp
)


if(UNIX)
    list(APPEND CORE_SOURCES implementation/file_lock_posix.cpp)
elseif(WIN32)
    list(APPEND CORE_SOURCES implementation/file_lock_win.cpp)
else()
    message(FATAL "No file lock implementation for your operating system.")
endif()


set(CORE_HEADERS
    base_tags.hpp
    callback_ptr.hpp
    containers_utils.hpp
    cross_thread_call.hpp
    disk_observer.hpp
    file_lock.hpp
    generic_concepts.hpp
    id.hpp
    ilogger.hpp
    ilogger_factory.hpp
    imedia_information.hpp
    iexif_reader.hpp
    itask_executor.hpp
    itasks_view.hpp
    iview_task.hpp
    iterator_wrapper.hpp
    logger.hpp
    logger_factory.hpp
    map_iterator.hpp
    media_types.hpp
    photo_information.hpp
    ptr_iterator.hpp
    search_expression_evaluator.hpp
    status.hpp
    stopwatch.hpp
    tag.hpp
    exif_reader_factory.hpp
    task_executor.hpp
    time_guardian.hpp
    tree.hpp
    implementation/aexif_reader.hpp
    implementation/exiv2_exif_reader.hpp
    implementation/log_file_rotator.hpp
    implementation/tree_iterator_base.hpp
    implementation/tree_level_iterator.hpp
    implementation/tree_private.hpp
    implementation/tree_flat_iterator.hpp
)

set(CORE_RESOURCE_FILES images/images.qrc)
set(CORE_MOC_FILES disk_observer.hpp)

qt5_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})
qt5_wrap_cpp(CORE_WRAPPED ${CORE_MOC_FILES})

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES} ${CORE_HEADERS} ${CORE_WRAPPED})
target_link_libraries(core
                        PRIVATE
                            system
                            ${OPENLIBRARY_LIBRARIES}
                            Qt5::Core
                            Qt5::Gui
                            ${EXIV2_LIBRARIES}
                            ${CMAKE_THREAD_LIBS_INIT}
                     )

target_include_directories(core SYSTEM PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                SYSTEM PRIVATE
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${OPENLIBRARY_INCLUDE_DIRS}
                                    ${EXIV2_INCLUDE_DIRS}
                                PRIVATE
                                    ${CMAKE_BINARY_DIR}/exports
                                    ${CMAKE_SOURCE_DIR}/src
)

generate_export_header(core)
hideSymbols(core)

if(BUILD_SHARED_LIBS)
    install(TARGETS core RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})
endif()

if(BUILD_TESTS)
    include(core_test.cmake)
endif()
