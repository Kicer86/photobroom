
find_package(OpenCV REQUIRED)
find_package(GTest  REQUIRED)
find_package(Qt6    REQUIRED COMPONENTS Gui)

find_program(Python python REQUIRED)


add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png
    COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${CMAKE_CURRENT_BINARY_DIR}/img1_500.png --width 500 --heigh 500
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png
    COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${CMAKE_CURRENT_BINARY_DIR}/img1_100.png --width 100 --heigh 100
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png
    COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.png --width 1000 --heigh 1000
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_500.jpeg
    COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${CMAKE_CURRENT_BINARY_DIR}/img1_500.jpeg --width 500 --heigh 500
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.jpeg
    COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${CMAKE_CURRENT_SOURCE_DIR}/img1.svg ${CMAKE_CURRENT_BINARY_DIR}/img1_1000.jpeg --width 1000 --heigh 1000
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
