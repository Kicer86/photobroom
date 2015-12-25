
#rules for finding StackWalker

if(EXISTS ${CMAKE_SOURCE_DIR}/3rd_party/StackWalker)

    if (NOT TARGET StackWalker)
        add_library(StackWalker STATIC ${CMAKE_SOURCE_DIR}/3rd_party/StackWalker/StackWalker.cpp)
    endif()

    set(STACKWALKER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd_party/StackWalker/)

endif()

set(STACKWALKER_LIBRARIES StackWalker )
set(STACKWALKER_INCLUDE_DIRS ${STACKWALKER_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(StackWalker DEFAULT_MSG STACKWALKER_INCLUDE_DIR)

mark_as_advanced(STACKWALKER_INCLUDE_DIR)
