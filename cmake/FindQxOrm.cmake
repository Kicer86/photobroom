
#rules for finding QxOrm
#based on http://www.vtk.org/Wiki/CMake:How_To_Find_Libraries

find_path(QXORM_INCLUDE_DIR QxOrm.h)

find_library(QXORM_LIBRARY NAMES QxOrm)

set(QXORM_LIBRARIES ${QXORM_LIBRARY} )
set(QXORM_INCLUDE_DIRS ${QXORM_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QxOrm DEFAULT_MSG QXORM_LIBRARY QXORM_INCLUDE_DIR)

mark_as_advanced(QXORM_INCLUDE_DIR QXORM_LIBRARY)
