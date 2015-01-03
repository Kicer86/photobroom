
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

setupTestEnvironmentFor(database)

set(SRC
        ${database_srcs}
        # model tests:

        # main()
        main.cpp
    )

add_executable(database_tests ${SRC})

target_link_libraries(database_tests PRIVATE ${database_link_libraries} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
turnOnCpp11(database_tests)
enableCodeCoverage(database_tests)

add_test(database database_tests)
