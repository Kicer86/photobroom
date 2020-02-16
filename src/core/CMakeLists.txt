
find_package(OpenLibrary 2.3  REQUIRED putils)
find_package(Qt5Core     5.10 REQUIRED)
find_package(Qt5Gui           REQUIRED)
find_package(Exiv2            REQUIRED)
find_package(Threads          REQUIRED)

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

set(CORE_SOURCES
    #interfaces
    iconfiguration.hpp
    icore_factory_accessor.hpp
    ifeatures_manager.hpp
    ilogger.hpp
    ilogger_factory.hpp
    ilogger.hpp
    ithumbnails_cache.hpp
    ithumbnails_manager.hpp
    imedia_information.hpp
    imodel_compositor_data_source.hpp
    iexif_reader.hpp
    ifeatures_manager.hpp
    itask_executor.hpp
    itasks_view.hpp
    iview_task.hpp

    #tiny utilities
    containers_utils.hpp
    function_wrappers.hpp
    generic_concepts.hpp
    id.hpp
    iterator_wrapper.hpp
    lazy_ptr.hpp
    map_iterator.hpp
    ptr_iterator.hpp
    status.hpp

    #utilities
    base_tags.hpp                                           implementation/base_tags.cpp
    configuration.hpp                                       implementation/configuration.cpp
    constants.hpp                                           implementation/constants.cpp
    core_factory_accessor.hpp                               implementation/core_factory_accessor.cpp
    disk_observer.hpp                                       implementation/disk_observer.cpp
    exif_reader_factory.hpp                                 implementation/exif_reader_factory.cpp
    ffmpeg_video_details_reader.hpp                         implementation/ffmpeg_video_details_reader.cpp
    features_manager.hpp                                    implementation/features_manager.cpp
    file_lock.hpp                                           implementation/file_lock.cpp
    image_tools.hpp                                         implementation/image_tools.cpp
    logger.hpp                                              implementation/logger.cpp
    logger_factory.hpp                                      implementation/logger_factory.cpp
    media_information.hpp                                   implementation/media_information.cpp
    media_types.hpp                                         implementation/media_types.cpp
    model_compositor.hpp                                    implementation/model_compositor.cpp
    oriented_image.hpp                                      implementation/oriented_image.cpp
    qmodelindex_comparator.hpp                              implementation/qmodelindex_comparator.cpp
    qmodelindex_selector.hpp                                implementation/qmodelindex_selector.cpp
    search_expression_evaluator.hpp                         implementation/search_expression_evaluator.cpp
    signal_postponer.hpp                                    implementation/signal_postponer.cpp
    stopwatch.hpp                                           implementation/stopwatch.cpp
    tag.hpp                                                 implementation/tag.cpp
    task_executor.hpp                                       implementation/task_executor.cpp
    task_executor_utils.hpp                                 implementation/task_executor_utils.cpp
    time_guardian.hpp                                       implementation/time_guardian.cpp
    thread_utils.hpp
    thumbnails_cache.hpp                                    implementation/thumbnails_cache.cpp
    thumbnail_generator.hpp                                 implementation/thumbnail_generator.cpp
    thumbnail_manager.hpp                                   implementation/thumbnail_manager.cpp

    # internal implementation
    implementation/aexif_reader.hpp                         implementation/aexif_reader.cpp
    implementation/configuration_p.hpp
    implementation/exiv2_exif_reader.hpp                    implementation/exiv2_exif_reader.cpp
    implementation/exiv2_media_information.hpp              implementation/exiv2_media_information.cpp
    implementation/ffmpeg_media_information.hpp             implementation/ffmpeg_media_information.cpp
    implementation/log_file_rotator.hpp                     implementation/log_file_rotator.cpp
)


if(UNIX)
    list(APPEND CORE_SOURCES implementation/file_lock_posix.cpp)
elseif(WIN32)
    list(APPEND CORE_SOURCES implementation/file_lock_win.cpp)
else()
    message(FATAL "No file lock implementation for your operating system.")
endif()

if(CMAKE_USE_PTHREADS_INIT)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_pthreads.cpp)
else()
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_null.cpp)
endif()

set(CORE_RESOURCE_FILES     images/images.qrc)

qt5_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})
qt5_wrap_cpp(CORE_WRAPPED ${CORE_MOC_FILES})

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES})
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
                                    ${CMAKE_SOURCE_DIR}/src
                                SYSTEM PRIVATE
                                    ${Qt5Gui_INCLUDE_DIRS}
                                    ${OPENLIBRARY_INCLUDE_DIRS}
                                    ${EXIV2_INCLUDE_DIRS}
                                PRIVATE
                                    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(core PROPERTIES AUTOMOC TRUE)

generate_export_header(core)
hideSymbols(core)

if(BUILD_SHARED_LIBS)
    install(TARGETS core RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})
endif()

if(BUILD_TESTING)
    include(core_test.cmake)
endif()
