
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)


set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/core)
setupTestEnvironmentFor(core ${BASE_DIR})

include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>)

set(SRC
        ${BASE_DIR}/implementation/tree.cpp

        tree_tests.cpp
    )

add_executable(core_tests ${SRC})

target_link_libraries(core_tests PRIVATE ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
enableCodeCoverage(core_tests)

add_test(core core_tests)

