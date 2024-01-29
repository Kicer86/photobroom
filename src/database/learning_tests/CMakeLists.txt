
find_package(OpenCV REQUIRED)
find_package(GTest  REQUIRED)
find_package(Qt6    REQUIRED COMPONENTS Gui)

find_program(Python python REQUIRED)

foreach(ext png jpeg)
    foreach(size 100 500 1000)
        convertSVG(${CMAKE_CURRENT_BINARY_DIR}/img1_${size}.${ext} ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${size} ${size})
    endforeach()
endforeach()

add_custom_target(opencv_tests_images
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png
        ${CMAKE_CURRENT_BINARY_DIR}/img1_100.jpeg
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
