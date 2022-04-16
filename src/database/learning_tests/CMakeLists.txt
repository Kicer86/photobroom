
find_package(OpenCV REQUIRED)
find_package(GTest  REQUIRED)
find_package(Qt6    REQUIRED COMPONENTS Gui)

find_program(Magick magick REQUIRED
    HINTS ${CMAKE_BINARY_DIR}/tools/ImageMagick
)


add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png
    COMMAND ${Magick} convert ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg -resize 500x ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png
    COMMAND ${Magick} convert ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg -resize 100x ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png
    COMMAND ${Magick} convert ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg -resize 1000x ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_500.jpeg
    COMMAND ${Magick} convert ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg -resize 500x -quality 1 ${CMAKE_CURRENT_BINARY_DIR}/img1_500.jpeg
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.jpeg
    COMMAND ${Magick} convert ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg -resize 1000x -quality 1 ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.jpeg
)

add_custom_target(opencv_tests_images
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_500.jpeg
        ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.jpeg
)

add_executable(
    opencv_tests
        phash_tests.cpp
)

target_link_libraries(
    opencv_tests
        PRIVATE
            GTest::gtest
            GTest::gmock
            GTest::gmock_main
            opencv_img_hash
            opencv_imgcodecs
            Qt6::Gui
)

target_include_directories(
    opencv_tests
        PRIVATE
            ${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(
    opencv_tests
        PRIVATE
            IMAGES_PATH="${CMAKE_CURRENT_BINARY_DIR}"
)

add_dependencies(opencv_tests opencv_tests_images)
add_test(NAME opencv_tests COMMAND opencv_tests)

set_tests_properties(opencv_tests PROPERTIES LABELS "LearningTest")
