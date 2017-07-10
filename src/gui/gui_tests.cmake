
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

include_directories(unit_tests)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/desktop/models/model_helpers/moc_idx_data_manager.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/desktop/models/model_helpers/moc_idxdata_deepfetcher.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/desktop/views/view_impl/moc_view_data_set.cpp PROPERTIES GENERATED TRUE)


addTestTarget(gui
                SOURCES
                    desktop/models/aphoto_info_model.cpp
                    desktop/models/db_data_model.cpp
                    desktop/models/model_helpers/idx_data.cpp
                    desktop/models/model_helpers/idx_data_manager.cpp
                    desktop/models/model_helpers/idxdata_deepfetcher.cpp
                    desktop/models/model_helpers/photos_matcher.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/desktop/models/model_helpers/moc_idx_data_manager.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/desktop/models/model_helpers/moc_idxdata_deepfetcher.cpp

                    desktop/views/view_impl/data.cpp
                    desktop/views/view_impl/model_index_info.cpp
                    desktop/views/view_impl/positions_calculator.cpp
                    desktop/views/view_impl/positions_reseter.cpp
                    desktop/views/view_impl/positions_translator.cpp
                    desktop/views/view_impl/view_data_set.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/desktop/views/view_impl/moc_view_data_set.cpp

                    # model tests:
                    unit_tests/model/idx_data_manager_tests.cpp
                    unit_tests/model/idx_data_tests.cpp

                    # view tests
                    unit_tests/view/data_tests.cpp
                    unit_tests/view/positions_calculator_tests.cpp
                    unit_tests/view/positions_reseter_tests.cpp

                    # helpers:
                    unit_tests/test_helpers/mock_configuration.hpp
                    unit_tests/test_helpers/mock_qabstractitemmodel.hpp

                    unit_tests/test_helpers/internal_task_executor.cpp
                    unit_tests/test_helpers/internal_task_executor.hpp

                    unit_tests/test_helpers/photo_info_model.cpp
                    unit_tests/test_helpers/photo_info_model.hpp

                    # main()
                    unit_tests/main.cpp

                LIBRARIES
                     PRIVATE
                        configuration
                        core
                        database
                        photos_crawler
                        Qt5::Core
                        Qt5::Gui
                        ${GMOCK_LIBRARY}
                        ${GTEST_LIBRARY}
                        ${CMAKE_THREAD_LIBS_INIT}

                INCLUDES
                    PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
)
