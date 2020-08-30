
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Test)
find_package(GTest REQUIRED CONFIG)

include_directories(unit_tests)

addTestTarget(gui
                SOURCES
                    desktop/models/aphoto_info_model.cpp
                    desktop/models/flat_model.cpp

                    desktop/utils/model_index_utils.cpp

                    # model tests:
                    unit_tests/model/aphoto_info_model_tests.cpp
                    unit_tests/model/flat_model_tests.cpp

                    # helpers:
                    unit_tests/test_helpers/mock_configuration.hpp
                    unit_tests/test_helpers/mock_qabstractitemmodel.hpp

                    unit_tests/test_helpers/internal_task_executor.cpp
                    unit_tests/test_helpers/internal_task_executor.hpp

                    unit_tests/test_helpers/photo_info_model.cpp
                    unit_tests/test_helpers/photo_info_model.hpp

                    # utils:
                    unit_tests/utils/model_index_utils_tests.cpp

                    # main()
                    unit_tests/main.cpp

                LIBRARIES
                    core
                    database
                    database_memory_backend
                    photos_crawler
                    sample_dbs
                    Qt5::Core
                    Qt5::Gui
                    Qt5::Widgets
                    Qt5::Test
                    GTest::gtest
                    GTest::gmock

                INCLUDES
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/models/model_helpers
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/models
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop
                    ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests
                    ${CMAKE_CURRENT_SOURCE_DIR}
)
