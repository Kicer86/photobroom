
find_package(OpenCV REQUIRED)
find_package(Qt6              REQUIRED COMPONENTS Core Gui Multimedia)
find_package(exiv2            REQUIRED)
find_package(Threads          REQUIRED)
find_package(CsLibGuarded     REQUIRED)

include(GenerateExportHeader)

set(CORE_SOURCES
    implementation/aexif_reader.hpp                         implementation/aexif_reader.cpp
    implementation/configuration_p.hpp
    implementation/exiftool_video_details_reader.hpp        implementation/exiftool_video_details_reader.cpp
    implementation/exiv2_exif_reader.hpp                    implementation/exiv2_exif_reader.cpp
    implementation/image_media_information.hpp              implementation/image_media_information.cpp
    implementation/init.cpp
    implementation/log_file_rotator.hpp                     implementation/log_file_rotator.cpp
    implementation/video_media_information.hpp              implementation/video_media_information.cpp
    accumulative_queue.hpp
    base_tags.hpp                                           implementation/base_tags.cpp
    configuration.hpp                                       implementation/configuration.cpp
    constants.hpp                                           implementation/constants.cpp
    containers_utils.hpp
    core_factory_accessor.hpp                               implementation/core_factory_accessor.cpp
                                                            implementation/data_from_path_extractor.cpp
    data_ptr.hpp
    disk_observer.hpp                                       implementation/disk_observer.cpp
    exif_reader_factory.hpp                                 implementation/exif_reader_factory.cpp
    features_manager_compositor.hpp                         implementation/features_manager_compositor.cpp
    function_wrappers.hpp
    generic_concepts.hpp
    iconfiguration.hpp
    icore_factory_accessor.hpp
    id.hpp
    iexif_reader.hpp
    ifeatures_manager.hpp
    ifeatures_manager.hpp
    ilogger_factory.hpp
    ilogger.hpp
    ilogger.hpp
    image_tools.hpp                                         implementation/image_tools.cpp
    imedia_information.hpp
    imodel_compositor_data_source.hpp
    itask_executor.hpp
    itasks_view.hpp
    itasks_view_utils.hpp                                   implementation/itasks_view_utils.cpp
    ithumbnails_generator.hpp
    iview_task.hpp
    lazy_ptr.hpp
    logger_factory.hpp                                      implementation/logger_factory.cpp
    logger.hpp                                              implementation/logger.cpp
    media_information.hpp                                   implementation/media_information.cpp
    media_types.hpp                                         implementation/media_types.cpp
    model_compositor.hpp                                    implementation/model_compositor.cpp
    observable_executor.hpp                                 implementation/observable_executor.cpp
    observables_registry.hpp                                implementation/observables_registry.cpp
    observable_task_executor.hpp
    oriented_image.hpp                                      implementation/oriented_image.cpp
    qmodelindex_comparator.hpp                              implementation/qmodelindex_comparator.cpp
    qmodelindex_selector.hpp                                implementation/qmodelindex_selector.cpp
    search_expression_evaluator.hpp                         implementation/search_expression_evaluator.cpp
    signal_postponer.hpp                                    implementation/signal_postponer.cpp
    slicer.hpp
    status.hpp
    tag.hpp                                                 implementation/tag.cpp
    tags_utils.hpp                                          implementation/tags_utils.cpp
    task_executor.hpp                                       implementation/task_executor.cpp
    task_executor_traits.hpp
    task_executor_utils.hpp                                 implementation/task_executor_utils.cpp
    thread_utils.hpp
    thumbnail_generator.hpp                                 implementation/thumbnail_generator.cpp
    time_guardian.hpp                                       implementation/time_guardian.cpp
    ts_queue.hpp
    utils.hpp
)

if(CMAKE_USE_PTHREADS_INIT AND NOT APPLE)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_pthreads.cpp)
else()
    set(CORE_SOURCES ${CORE_SOURCES} implementation/thread_utils_null.cpp)
endif()

set(CORE_RESOURCE_FILES     images/images.qrc)

qt6_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})

add_library(core ${CORE_SOURCES} ${CORE_RESOURCES})
target_link_libraries(core
                        PUBLIC
                            Qt::Core
                            Qt::Gui
                            Qt::Multimedia
                        PRIVATE
                            system
                            ${CMAKE_THREAD_LIBS_INIT}
                            CsLibGuarded::CsLibGuarded
                            exiv2lib
                            opencv_videoio
)

target_include_directories(core PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_SOURCE_DIR}/src
                                PRIVATE
                                    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(core PROPERTIES AUTOMOC TRUE)

generate_export_header(core)
hideSymbols(core)

if(BUILD_SHARED_LIBS)
    install(TARGETS core RUNTIME DESTINATION ${PATH_LIBS}
                         LIBRARY DESTINATION ${PATH_LIBS})

    objdump_target(core)
endif()

if(BUILD_TESTING)
    include(core_test.cmake)
endif()
