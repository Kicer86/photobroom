
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Qt5Core REQUIRED)


set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/photos_crawler)
setupTestEnvironmentFor(photos_crawler ${BASE_DIR})

include_directories($<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>)

set(SRC
        ${photos_crawler_srcs}
        analyzerTests.cpp
        photo_crawler_tests.cpp
        photo_crawler_builder_tests.cpp
    )

add_executable(photos_crawler_tests ${SRC})

target_link_libraries(photos_crawler_tests PRIVATE Qt5::Core ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
enableCodeCoverage(photos_crawler_tests)

add_test(photos_crawler photos_crawler_tests)
