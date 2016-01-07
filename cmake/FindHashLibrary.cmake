
#rules for finding HashLibrary
find_path(HASHLIBRARY_INCLUDE_DIR hash-library/sha256.h)
find_library(HASHLIBRARY_LIBRARY NAMES hashlibrary)

set(HASHLIBRARY_LIBRARIES ${HASHLIBRARY_LIBRARY} )
set(HASHLIBRARY_INCLUDE_DIRS ${HASHLIBRARY_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HashLibrary DEFAULT_MSG HASHLIBRARY_LIBRARY HASHLIBRARY_INCLUDE_DIR)

mark_as_advanced(HASHLIBRARY_INCLUDE_DIR HASHLIBRARY_LIBRARY)
