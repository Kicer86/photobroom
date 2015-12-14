
#rules for finding libbacktrace

if(EXISTS ${CMAKE_SOURCE_DIR}/3rd_party/libbacktrace)

    add_subdirectory(${CMAKE_SOURCE_DIR}/3rd_party/libbacktrace ${CMAKE_BINARY_DIR}/3rd_party/libbacktrace)

    set(LIBBACKTRACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd_party/libbacktrace ${CMAKE_BINARY_DIR}/3rd_party)

else()

    # TODO?

endif()

set(LIBBACKTRACE_LIBRARIES backtrace )
set(LIBBACKTRACE_INCLUDE_DIRS ${LIBBACKTRACE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibBacktrace DEFAULT_MSG LIBBACKTRACE_INCLUDE_DIR)

mark_as_advanced(LIBBACKTRACE_INCLUDE_DIR)
