

#rules for finding Exiv2
#based on http://www.vtk.org/Wiki/CMake:How_To_Find_Libraries

find_path(EXIV2_INCLUDE_DIR exiv2/exiv2.hpp)

find_library(EXIV2_LIBRARY NAMES exiv2 exiv2d)

set(EXIV2_LIBRARIES ${EXIV2_LIBRARY} )
set(EXIV2_INCLUDE_DIRS ${EXIV2_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Exiv2 DEFAULT_MSG EXIV2_LIBRARY EXIV2_INCLUDE_DIR)

mark_as_advanced(EXIV2_INCLUDE_DIR EXIV2_LIBRARY)
