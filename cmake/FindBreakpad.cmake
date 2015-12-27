
#rules for finding Breakpad

if(EXISTS ${CMAKE_SOURCE_DIR}/3rd_party/breakpad)

    if (NOT TARGET breakpad)

        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/breakpad/install/usr/lib/libbreakpad.a

                           COMMAND ${CMAKE_SOURCE_DIR}/3rd_party/breakpad/configure --prefix=/usr
                           COMMAND make
                           COMMAND make install DESTDIR=${CMAKE_BINARY_DIR}/3rd_party/breakpad/install

                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/breakpad
                          )

        add_custom_target(breakpad_build
                          DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/breakpad/install/usr/lib/libbreakpad.a)

        add_library(breakpad STATIC IMPORTED)
        set_target_properties(breakpad PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/3rd_party/breakpad/install/usr/lib/libbreakpad.a)
        add_dependencies(breakpad breakpad_build)

    endif()

    set(BREAKPAD_INCLUDE_DIR ${CMAKE_BINARY_DIR}/3rd_party/breakpad/install/usr/include)

endif()

set(BREAKPAD_LIBRARIES breakpad )
set(BREAKPAD_INCLUDE_DIRS ${BREAKPAD_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Breakpad DEFAULT_MSG BREAKPAD_INCLUDE_DIR)

mark_as_advanced(BREAKPAD_INCLUDE_DIR)
