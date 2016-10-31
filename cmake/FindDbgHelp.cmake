
#rules for finding DbgHelp
# some tricks from:
# https://github.com/arx/ArxLibertatis/blob/master/cmake/FindDbgHelp.cmake

find_library(DBGHELP_LIBRARY NAMES dbghelp)

if(DBGHELP_LIBRARY)
    find_path(DBGHELP_INCLUDE_DIR dbghelp.h)
elseif(MSVC)
    set(DBGHELP_LIBRARY dbghelp)
    set(DBGHELP_INCLUDE_DIR)
endif()

set(DBGHELP_LIBRARIES ${DBGHELP_LIBRARY} )
set(DBGHELP_INCLUDE_DIRS ${DBGHELP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBGHELP DEFAULT_MSG DBGHELP_LIBRARY)

mark_as_advanced(DBGHELP_INCLUDE_DIR DBGHELP_LIBRARY)
