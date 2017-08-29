
cmake_policy(SET CMP0020 NEW)

configure_file(config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)

add_subdirectory(core)
add_subdirectory(crash_catcher)
add_subdirectory(photos_crawler)
add_subdirectory(database)
add_subdirectory(gui)
add_subdirectory(plugins)
add_subdirectory(project_utils)
add_subdirectory(system)
add_subdirectory(updater)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_BINARY_DIR}
)

include_directories(SYSTEM ${OPENLIBRARY_INCLUDES}
                           ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                   )

set(BROOM_SOURCES main.cpp
                  $<TARGET_OBJECTS:plugins>
)


# workaround for a bug https://developercommunity.visualstudio.com/content/problem/74173/cannot-use-some-functionalities-of-qt-framework-be.html
# also see: https://bugreports.qt.io/browse/QTBUG-61667
#
# + CMake missing implementations:
# https://stackoverflow.com/questions/44960715/how-to-enable-stdc17-in-vs2017-with-cmake
if(MSVC_VERSION)
    if(MSVC_VERSION GREATER_EQUAL 1900)

        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
            message("Enabling c++latest for Visual Studio")
        else()
            message(FATAL_ERROR "Enabling c++latest for Visual Studio is not possible")
        endif()
    else()
        message(FATAL_ERROR "Visual Studio 2017 is required")
    endif()
endif()


if(WIN32)

    set(EXE_FLAGS WIN32)
    configure_file(env/PhotoBroom.hpp.cmake PhotoBroom.hpp @ONLY)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    include_directories(${CMAKE_BINARY_DIR}/tools)                                 #for PhotoBroom.rc include files
    set(BROOM_SOURCES ${BROOM_SOURCES} env/PhotoBroom.rc)

else()
    set(EXE_FLAGS)
endif()

add_executable(photo_broom ${EXE_FLAGS} ${BROOM_SOURCES})
target_link_libraries(photo_broom
                        core
                        crash_catcher
                        database
                        gui
                        project_utils
                        system
                        Qt5::Core)

install(TARGETS photo_broom RUNTIME DESTINATION ${PATH_BIN})

if(WIN32)
    #photo_broom on Windows requires Images (for exe file ico)
    add_dependencies(photo_broom Images)
endif()
