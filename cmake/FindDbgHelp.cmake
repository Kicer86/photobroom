
#rules for finding DbgHelp

find_path(DBGHELP_INCLUDE_DIR dbghelp.h)
find_library(DBGHELP_LIBRARY NAMES DbgHelp)

set(DBGHELP_LIBRARIES ${DBGHELP_LIBRARY} )
set(DBGHELP_INCLUDE_DIRS ${DBGHELP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBGHELP DEFAULT_MSG DBGHELP_LIBRARY DBGHELP_INCLUDE_DIR)

mark_as_advanced(DBGHELP_INCLUDE_DIR DBGHELP_LIBRARY)
