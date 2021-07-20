
find_package(OpenLibrary 2.3  REQUIRED putils)
find_package(Qt6              REQUIRED COMPONENTS Core Gui)
find_package(exiv2            REQUIRED)
find_package(Threads          REQUIRED)

include(GenerateExportHeader)
include(${OPENLIBRARY_USE_FILE})

set(CORE_SOURCES
    #interfaces
    iconfiguration.hpp
    icore_factory_accessor.hpp
    ifeatures_manager.hpp
    ilogger.hpp
    ilogger_factory.hpp
    ilogger.hpp
    imedia_information.hpp
    imodel_compositor_data_source.hpp
    iexif_reader.hpp
    ifeatures_manager.hpp
    itask_executor.hpp
    itasks_view.hpp
    ithumbnails_generator.hpp
    iview_task.hpp

    #tiny utilities
    containers_utils.hpp
    function_wrappers.hpp
    generic_concepts.hpp
    id.hpp
    lazy_ptr.hpp
    status.hpp
    tags_utils.hpp                                          implementation/tags_utils.cpp
    task_executor_traits.hpp

    #utilities
    base_tags.hpp                                           implementation/base_tags.cpp
    configuration.hpp                                       implementation/configuration.cpp
    constants.hpp                                           implementation/constants.cpp
    core_factory_accessor.hpp                               implementation/core_factory_accessor.cpp
    disk_observer.hpp                                       implementation/disk_observer.cpp
    exif_reader_factory.hpp                                 implementation/exif_reader_factory.cpp
    ffmpeg_video_details_reader.hpp                         implementation/ffmpeg_video_details_reader.cpp
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
    thumbnail_generator.hpp                                 implementation/thumbnail_generator.cpp
    thread_utils.hpp

    # internal implementation
    implementation/aexif_reader.hpp                         implementation/aexif_reader.cpp
    implementation/configuration_p.hpp
    implementation/exiv2_exif_reader.hpp                    implementation/exiv2_exif_reader.cpp
    implementation/exiv2_media_information.hpp              implementation/exiv2_media_information.cpp
    implementation/ffmpeg_media_information.hpp             implementation/ffmpeg_media_information.cpp
    implementation/log_file_rotator.hpp                     implementation/log_file_rotator.cpp
)

if(CMAKE_USE_PTHREADS_INIT AND NOT APPLE)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_pthreads.cpp)
else()
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_null.cpp)
endif()

set(CORE_RESOURCE_FILES     images/images.qrc)

qt6_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES})
target_link_libraries(core
                        PUBLIC
                            Qt::Core
                            Qt::Gui
                        PRIVATE
                            system
                            ${OPENLIBRARY_LIBRARIES}
                            ${CMAKE_THREAD_LIBS_INIT}
                            exiv2lib
)

target_include_directories(core PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
                                SYSTEM PRIVATE
                                    ${OPENLIBRARY_INCLUDE_DIRS}
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
