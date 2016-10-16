
find_package(Boost REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED putils)
find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(EasyExif REQUIRED)
find_package(Threads REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

# some classes in core are using interface types of database. Is it ok? - to consider
include_directories($<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/atagfeeder.cpp
    implementation/base_tags.cpp
    implementation/cross_thread_callback.cpp
    implementation/disk_observer.cpp
    implementation/file_lock.cpp
    implementation/ilogger.cpp
    implementation/log_file_rotator.cpp
    implementation/logger.cpp
    implementation/logger_factory.cpp
    implementation/photos_manager.cpp
    implementation/search_expression_evaluator.cpp
    implementation/tag.cpp
    implementation/tag_feeder_factory.cpp
    implementation/tag_updater.cpp
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
    cross_thread_callback.hpp
    disk_observer.hpp
    file_lock.hpp
    ilogger.hpp
    ilogger_factory.hpp
    iphotos_manager.hpp
    itagfeeder.hpp
    itask_executor.hpp
    itasks_view.hpp
    iview_task.hpp
    logger.hpp
    logger_factory.hpp
    map_iterator.hpp
    photos_manager.hpp
    search_expression_evaluator.hpp
    status.hpp
    tag.hpp
    tag_feeder_factory.hpp
    tag_updater.hpp
    task_executor.hpp
    time_guardian.hpp
    tree.hpp
    implementation/atagfeeder.hpp
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

list(APPEND CORE_SOURCES implementation/easy_exif_tag_feeder.cpp)
list(APPEND CORE_HEADERS implementation/easy_exif_tag_feeder.hpp)

#addSourceFlags(implementation/photo_info.cpp COMPILE_FLAGS -fgnu-tm)

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES} ${CORE_HEADERS} ${CORE_WRAPPED})
target_link_libraries(core
                        PRIVATE
                            system
                            ${OPENLIBRARY_LIBRARIES}
                            Qt5::Core
                            Qt5::Gui
                            ${EASYEXIF_LIBRARIES}
                            ${CMAKE_THREAD_LIBS_INIT}
                     )

target_include_directories(core SYSTEM PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                    ${Boost_INCLUDE_DIRS}
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                SYSTEM PRIVATE
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${OPENLIBRARY_INCLUDE_DIRS}
                                    ${EASYEXIF_INCLUDE_DIR}
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

