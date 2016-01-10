
#rules for finding EasyExif
find_path(EASYEXIF_INCLUDE_DIR easyexif/exif.h)
find_library(EASYEXIF_LIBRARY NAMES easyexif)
    
set(EASYEXIF_LIBRARIES ${EASYEXIF_LIBRARY} )
set(EASYEXIF_INCLUDE_DIRS ${EASYEXIF_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EasyExif DEFAULT_MSG EASYEXIF_LIBRARY EASYEXIF_INCLUDE_DIR)

mark_as_advanced(EASYEXIF_INCLUDE_DIR EASYEXIF_LIBRARY)
