
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test Quick)
find_package(GTest REQUIRED CONFIG)
find_program(QML_TEST_RUNNER
             NAMES qmltestrunner-qt5 qmltestrunner
             PATHS /usr/lib/qt6/bin
             REQUIRED
)

include_directories(unit_tests)

addTestTarget(gui
                SOURCES
                    desktop/models/aheavy_list_model.hpp                   # needed for moc
                    desktop/models/aphoto_data_model.cpp
                    desktop/models/flat_model.cpp
                    desktop/utils/model_index_utils.cpp
                    desktop/quick_items/selection_manager_component.cpp
                    desktop/utils/thumbnail_manager.cpp
                    desktop/utils/thumbnails_cache.cpp

                    # model tests:
                    unit_tests/model/aheavy_list_model_tests.cpp
                    unit_tests/model/aphoto_data_model_tests.cpp
                    unit_tests/model/flat_model_tests.cpp

                    # helpers:
                    unit_tests/test_helpers/mock_configuration.hpp
                    unit_tests/test_helpers/mock_qabstractitemmodel.hpp

                    unit_tests/test_helpers/internal_task_executor.cpp
                    unit_tests/test_helpers/internal_task_executor.hpp

                    # utils:
                    unit_tests/utils/model_index_utils_tests.cpp
                    unit_tests/utils/selection_manager_component_tests.cpp
                    unit_tests/utils/thumbnails_manager_tests.cpp
                    unit_tests/utils/thumbnails_cache_tests.cpp

                    # main()
                    unit_tests/main.cpp

                LIBRARIES
                    core
                    database
                    database_memory_backend
                    photos_crawler
                    sample_dbs
                    Qt::Core
                    Qt::Quick
                    Qt::Test
                    GTest::gtest
                    GTest::gmock

                INCLUDES
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/models/model_helpers
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/models
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop
                    ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests
                    ${CMAKE_CURRENT_SOURCE_DIR}

                DEFINITIONS
                    GUI_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)


add_test(
    NAME QmlLogicTests
    COMMAND ${QML_TEST_RUNNER} -input ${PROJECT_SOURCE_DIR}/src/gui/unit_tests/ui/ZoomLogicTests.qml
)

set_tests_properties(QmlLogicTests PROPERTIES LABELS "UnitTest")

