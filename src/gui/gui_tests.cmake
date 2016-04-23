
find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

include_directories(SYSTEM ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

include_directories(unit_tests)

set(SRC
        $<TARGET_OBJECTS:gui_models>
        $<TARGET_OBJECTS:gui_views>
        $<TARGET_OBJECTS:gui_utils>

        # model tests:
        unit_tests/model/idx_data_manager_tests.cpp
        unit_tests/model/idx_data_tests.cpp

        # view tests
        unit_tests/view/data_tests.cpp
        unit_tests/view/positions_calculator_tests.cpp
        unit_tests/view/positions_reseter_tests.cpp

        # helpers:
        unit_tests/test_helpers/mock_configuration.hpp
        unit_tests/test_helpers/mock_database.hpp
        unit_tests/test_helpers/mock_qabstractitemmodel.hpp

        unit_tests/test_helpers/internal_task_executor.cpp
        unit_tests/test_helpers/internal_task_executor.hpp

        # main()
        unit_tests/main.cpp
    )

add_executable(gui_tests ${SRC})

target_link_libraries(gui_tests
                        PRIVATE
                            configuration
                            core
                            database
                            photos_crawler
                            project_utils
                            ${OPENLIBRARY_LIBRARIES}
                            Qt5::Core
                            ${GMOCK_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
)

enableCodeCoverage(gui_tests)

add_test(gui gui_tests)
