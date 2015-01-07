
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/database/backends/sql_backends)
setupTestEnvironmentFor(sql_backend_base ${BASE_DIR})

set(SRC
        ${sql_backend_base_srcs}
        sql_select_query_generator_tests.cpp

        # main()
        main.cpp
    )

add_executable(sql_backend_base_tests ${SRC})

target_link_libraries(sql_backend_base_tests PRIVATE ${sql_backend_base_link_libraries} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
enableCodeCoverage(sql_backend_base_tests)

add_test(sql_backend_base sql_backend_base_tests)
