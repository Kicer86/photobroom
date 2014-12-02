
find_package(Threads REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenLibrary 2.0 REQUIRED utils QtExt)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

set(BASE_DIR ${CMAKE_SOURCE_DIR}/src/gui)

get_target_property(gui_include_dirs gui INCLUDE_DIRECTORIES)
get_target_property(gui_raw_srcs gui SOURCES)
get_target_property(gui_link_libraries gui LINK_LIBRARIES)

include_directories(${gui_include_dirs} ${BASE_DIR})
include_directories(SYSTEM ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

foreach(source ${gui_raw_srcs})
    if (IS_ABSOLUTE ${source})
        list(APPEND gui_srcs ${source})
    else()    
        list(APPEND gui_srcs ${BASE_DIR}/${source})
    endif()
endforeach()

set(SRC
        ${gui_srcs}
        positions_calculator_tests.cpp
        positions_reseter_tests.cpp
        main.cpp

        mock_configuration.hpp
        mock_qabstractitemmodel.hpp
    )

add_executable(gui_tests ${SRC} ${GUI_WRAPPED_CPP})

target_link_libraries(gui_tests PRIVATE ${gui_link_libraries} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
turnOnCpp11(gui_tests)
enableCodeCoverage(gui_tests)

add_test(gui gui_tests)
