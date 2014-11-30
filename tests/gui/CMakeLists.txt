
find_package(Threads REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenLibrary 2.0 REQUIRED utils)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/gui)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/gui/Qt5 ${CMAKE_BINARY_DIR}/exports ${BASE_DIR}/Qt5/model_view/view_helpers/)


qt5_wrap_cpp(GUI_WRAPPED_CPP)

get_target_property(gui_raw_srcs gui SOURCES)

foreach(source ${gui_raw_srcs})
    if (IS_ABSOLUTE ${source})
        list(APPEND gui_srcs ${source})
    else()    
        list(APPEND gui_srcs ${CMAKE_SOURCE_DIR}/src/gui/${source})
    endif()
endforeach()

set(SRC
        ${gui_srcs}
        positions_calculator_tests.cpp
        positions_reseter_tests.cpp

        mock_configuration.hpp
        mock_qabstractitemmodel.hpp
    )

add_executable(gui_tests ${SRC} ${GUI_WRAPPED_CPP})

target_link_libraries(gui_tests PRIVATE configuration Qt5::Core Qt5::Gui ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
turnOnCpp11(gui_tests)
enableCodeCoverage(gui_tests)

add_test(gui gui_tests)
