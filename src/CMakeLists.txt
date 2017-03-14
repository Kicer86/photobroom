
cmake_policy(SET CMP0020 NEW)

configure_file(config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)

add_subdirectory(configuration)
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
                        configuration
                        gui
                        project_utils
                        system
                        Qt5::Core)

install(TARGETS photo_broom RUNTIME DESTINATION ${PATH_BIN})

if(WIN32)
    #photo_broom on Windows requires Images (for exe file ico)
    add_dependencies(photo_broom Images)
endif()
