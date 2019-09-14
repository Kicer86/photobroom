
add_executable(enum_to_string main.cpp)

if(BUILD_TESTS)
    include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

    addTestTarget(enum_to_string
                    SOURCES
                        unit_test.cpp
                    LIBRARIES
                        PRIVATE
                            ${GMOCK_MAIN_LIBRARY}
                            ${GMOCK_LIBRARY}
                            ${GTEST_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
                    INCLUDES
                        SYSTEM PRIVATE
                            ${GMOCK_INCLUDE_DIRS}
                            ${GTEST_INCLUDE_DIRS}
    )
endif()
