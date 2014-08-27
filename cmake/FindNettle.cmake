

#rules for finding Nettle

find_path(NETTLE_INCLUDE_DIR nettle/aes.h)
find_library(NETTLE_LIBRARY NAMES nettle)

set(NETTLE_LIBRARIES ${NETTLE_LIBRARY} )
set(NETTLE_INCLUDE_DIRS ${NETTLE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Nettle DEFAULT_MSG NETTLE_LIBRARY NETTLE_INCLUDE_DIR)

mark_as_advanced(NETTLE_INCLUDE_DIR NETTLE_LIBRARY)
