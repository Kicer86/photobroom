
find_package(Threads REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary 2.0 REQUIRED utils)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/gui)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR}/exports ${BASE_DIR}/Qt5/model_view/view_helpers/)


qt5_wrap_cpp(GUI_WRAPPED_CPP)

set(SRC
        ${BASE_DIR}/Qt5/model_view/view_helpers/data.cpp
        ${BASE_DIR}/Qt5/model_view/view_helpers/positions_calculator.cpp
        ${BASE_DIR}/Qt5/model_view/view_helpers/positions_reseter.cpp
        positions_calculator_tests.cpp
        positions_reseter_tests.cpp
        
        mock_configuration.hpp
    )

add_executable(gui_tests ${SRC} ${GUI_WRAPPED_CPP})

target_link_libraries(gui_tests PRIVATE configuration Qt5::Core Qt5::Gui ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
turnOnCpp11(gui_tests)

add_test(gui gui_tests)
