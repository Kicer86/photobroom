
find_package(OpenLibrary 2.3 REQUIRED putils)
find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(Threads REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(pybind11 REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

# check some required things
include(CheckTypeSize)
set(CMAKE_REQUIRED_INCLUDES   ${Qt5Core_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})
set(CMAKE_EXTRA_INCLUDE_FILES QtGlobal json/config.h)
set(CMAKE_REQUIRED_LIBRARIES  Qt5::Core)

check_type_size("qlonglong"   QLONGLONG  LANGUAGE CXX)
check_type_size("Json::Int64" JSON_INT64 LANGUAGE CXX)

if (NOT QLONGLONG EQUAL JSON_INT64)
    message(FATAL_ERROR "qlonglong and Json::Int64 do not have same size")
endif()
##

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/aexif_reader.cpp
    implementation/base_tags.cpp
    implementation/configuration.cpp
    implementation/constants.cpp
    implementation/core_factory_accessor.cpp
    implementation/cross_thread_callback.cpp
    implementation/disk_observer.cpp
    implementation/exif_reader_factory.cpp
    implementation/exiv2_exif_reader.cpp
    implementation/exiv2_media_information.cpp
    implementation/ffmpeg_video_details_reader.cpp
    implementation/ffmpeg_media_information.cpp
    implementation/file_lock.cpp
    implementation/ilogger.cpp
    implementation/image_tools.cpp
    implementation/log_file_rotator.cpp
    implementation/logger.cpp
    implementation/logger_factory.cpp
    implementation/media_information.cpp
    implementation/media_types.cpp
    implementation/python_thread.cpp
    implementation/qmodelindex_comparator.cpp
    implementation/qmodelindex_selector.cpp
    implementation/search_expression_evaluator.cpp
    implementation/signal_postponer.cpp
    implementation/stopwatch.cpp
    implementation/tag.cpp
    implementation/task_executor.cpp
    implementation/task_executor_utils.cpp
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
    configuration.hpp
    constants.hpp
    containers_utils.hpp
    core_factory_accessor.hpp
    cross_thread_call.hpp
    disk_observer.hpp
    ffmpeg_video_details_reader.hpp
    file_lock.hpp
    generic_concepts.hpp
    iconfiguration.hpp
    icore_factory_accessor.hpp
    id.hpp
    ilogger.hpp
    ilogger_factory.hpp
    image_tools.hpp
    imedia_information.hpp
    iexif_reader.hpp
    itask_executor.hpp
    itasks_view.hpp
    iview_task.hpp
    iterator_wrapper.hpp
    logger.hpp
    logger_factory.hpp
    map_iterator.hpp
    media_information.hpp
    media_types.hpp
    ptr_iterator.hpp
    python_thread.hpp
    qmodelindex_comparator.hpp
    qmodelindex_selector.hpp
    search_expression_evaluator.hpp
    signal_postponer.hpp
    status.hpp
    stopwatch.hpp
    tag.hpp
    exif_reader_factory.hpp
    task_executor.hpp
    task_executor_utils.hpp
    time_guardian.hpp
    universal_operators.hpp
    implementation/aexif_reader.hpp
    implementation/configuration_p.hpp
    implementation/exiv2_exif_reader.hpp
    implementation/exiv2_media_information.hpp
    implementation/ffmpeg_media_information.hpp
    implementation/log_file_rotator.hpp
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
                            ${JSONCPP_LIBRARIES}
                            pybind11::embed
)

target_include_directories(core SYSTEM PUBLIC
                                    ${Qt5Core_INCLUDE_DIRS}
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                SYSTEM PRIVATE
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${OPENLIBRARY_INCLUDE_DIRS}
                                    ${EXIV2_INCLUDE_DIRS}
                                    ${JSONCPP_INCLUDE_DIRS}
                                PRIVATE
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
