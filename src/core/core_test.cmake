
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(exiv2   REQUIRED)
find_package(FFmpeg  COMPONENTS AVCODEC AVFORMAT AVUTIL REQUIRED)
find_package(GTest   REQUIRED CONFIG)
find_package(Qt6     REQUIRED COMPONENTS Core Gui)
find_package(Qt6Test REQUIRED)
find_package(OpenCV  REQUIRED)

find_program(PYTHON python REQUIRED)
find_program(WGET wget REQUIRED)

foreach(ext png jpeg)
    foreach(img img1 img2)
        convertSVG(${CMAKE_CURRENT_BINARY_DIR}/${img}.${ext} ${PROJECT_SOURCE_DIR}/src/unit_tests_utils/assets/${img}.svg -1 -1)
    endforeach()
endforeach()

add_custom_command(OUTPUT alterd_images/generated
    COMMAND ${PYTHON} ARGS ${CMAKE_CURRENT_SOURCE_DIR}/core_test.py img1.png alterd_images
    COMMAND ${PYTHON} ARGS ${CMAKE_CURRENT_SOURCE_DIR}/core_test.py img2.png alterd_images
    COMMAND ${CMAKE_COMMAND} -E copy img1.png alterd_images/img1_.png
    COMMAND ${CMAKE_COMMAND} -E copy img2.png alterd_images/img2_.png
    COMMAND ${CMAKE_COMMAND} -E touch alterd_images/generated
    DEPENDS img1.png
    DEPENDS img2.png
)

add_custom_target(core_tests_images
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/img1.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1.jpeg
        ${CMAKE_CURRENT_BINARY_DIR}/img2.png
        ${CMAKE_CURRENT_BINARY_DIR}/img2.jpeg
        alterd_images/generated
)

if (BUILD_LEARNING_TESTS)

    # download face dataset from https://www.easyhdr.com/examples/wadi-rum-sunset/

    set(hdr_photos_list)
    foreach(N 1 2 3)
        add_custom_command(OUTPUT wadi-rum-sunset${N}.jpg
                           COMMAND ${WGET} https://www.easyhdr.com/examples/wadi-rum-sunset/wadi-rum-sunset${N}.jpg
                                           -O ${CMAKE_CURRENT_BINARY_DIR}/wadi-rum-sunset${N}.jpg
        )

        list(APPEND hdr_photos_list wadi-rum-sunset${N}.jpg)
    endforeach()

    foreach(URL
        https://upload.wikimedia.org/wikipedia/commons/0/09/StLouisArchMultExpEV-4.72.JPG
        https://upload.wikimedia.org/wikipedia/commons/c/c3/StLouisArchMultExpEV-1.82.JPG
        https://upload.wikimedia.org/wikipedia/commons/8/89/StLouisArchMultExpEV+1.51.JPG
        https://upload.wikimedia.org/wikipedia/commons/8/8f/StLouisArchMultExpEV+4.09.JPG
    )
        get_filename_component(fileName ${URL} NAME)

        add_custom_command(OUTPUT ${fileName}
                           COMMAND ${WGET} ${URL} -O ${CMAKE_CURRENT_BINARY_DIR}/${fileName}
        )

        list(APPEND hdr_photos_list ${fileName})
    endforeach()

    add_custom_target(hdr_photos
        DEPENDS
            ${hdr_photos_list}
    )

    add_executable(core_lt
        learning_tests/hdr_assembler_tests.cpp
        learning_tests/image_aligner_tests.cpp
    )

    target_include_directories(core_lt
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(core_lt
        PRIVATE
            core

            Qt::Core
            opencv_photo
            opencv_tracking
            GTest::gtest
            GTest::gmock
            GTest::gmock_main
    )

    add_dependencies(core_lt
        core_tests_images
        hdr_photos
    )

    add_test(
        NAME core_learning_tests
        COMMAND core_lt
    )

    set_tests_properties(core_learning_tests PROPERTIES LABELS "LearningTest")
endif()


# for exif testing (https://github.com/golang/go/issues/4341)
add_custom_command(OUTPUT exif_tests_images.tar
                   COMMAND ${WGET} https://storage.googleapis.com/go-attachment/4341/0/f.tar -O exif_tests_images.tar
)

add_custom_command(OUTPUT f/f1.jpg
                   COMMAND ${CMAKE_COMMAND} -E tar xf exif_tests_images.tar
                   DEPENDS exif_tests_images.tar
)

add_custom_target(exif_tests_images
    DEPENDS
        f/f1.jpg
)


add_executable(core_ut
    implementation/aexif_reader.cpp
    implementation/base_tags.cpp
    implementation/exiv2_exif_reader.cpp
    implementation/exif_reader_factory.cpp
    implementation/data_from_path_extractor.cpp
    implementation/oriented_image.cpp
    implementation/image_aligner.cpp
    implementation/image_media_information.cpp
    implementation/media_information.cpp
    implementation/media_types.cpp
    implementation/model_compositor.cpp
    implementation/qmodelindex_selector.cpp
    implementation/qmodelindex_comparator.cpp
    implementation/video_media_information.cpp
    implementation/tag.cpp
    implementation/task_executor_utils.cpp
    imodel_compositor_data_source.hpp

    unit_tests/containers_utils_tests.cpp
    unit_tests/data_from_path_extractor_tests.cpp
    unit_tests/function_wrappers_tests.cpp
    unit_tests/image_aligner_tests.cpp
    unit_tests/json_serializer_tests.cpp
    unit_tests/lazy_ptr_tests.cpp
    unit_tests/model_compositor_tests.cpp
    unit_tests/oriented_image_tests.cpp
    unit_tests/qmodelindex_comparator_tests.cpp
    unit_tests/qmodelindex_selector_tests.cpp
    unit_tests/status_tests.cpp
    unit_tests/tag_value_tests.cpp
)

target_link_libraries(core_ut
    PRIVATE
        ${EXIV2_LIB}
        ${FFMPEG_LIBRARIES}
        GTest::gtest
        GTest::gmock_main
        Qt::Core
        Qt::Gui
        Qt::Test
        opencv_tracking
)

target_include_directories(core_ut
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/third_party/reflect-cpp/include
        ${PROJECT_BINARY_DIR}
)

target_compile_definitions(core_ut
    PRIVATE
        CORE_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)

set_target_properties(core_ut PROPERTIES AUTOMOC TRUE)
add_dependencies(core_ut
    core_tests_images
    exif_tests_images
)

add_test(
    NAME core
    COMMAND core_ut
)

register_unit_test(core core_ut)


add_executable(core_video_ut
    implementation/aexif_reader.cpp
    implementation/base_tags.cpp
    implementation/exiv2_exif_reader.cpp
    implementation/exif_reader_factory.cpp
    implementation/image_media_information.cpp
    implementation/media_information.cpp
    implementation/media_types.cpp
    implementation/tag.cpp
    implementation/video_media_information.cpp

    unit_tests/video_metadata_extraction_tests.cpp
)

target_link_libraries(core_video_ut
    PRIVATE
        ${EXIV2_LIB}
        ${FFMPEG_LIBRARIES}
        GTest::gtest
        GTest::gmock_main
        Qt::Core
        Qt::Gui
        opencv_tracking
)

target_include_directories(core_video_ut
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
)

target_compile_definitions(core_video_ut
    PRIVATE
        CORE_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)

set_target_properties(core_video_ut PROPERTIES AUTOMOC TRUE)

add_test(
    NAME core_video
    COMMAND core_video_ut
)

register_unit_test(core_video core_video_ut "HeavyData")
