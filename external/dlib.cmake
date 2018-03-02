
file(GLOB libraries
     LIST_DIRECTORIES false
     "${PROJECT_BINARY_DIR}/dlib/dlib.cpython-*"
)

file(INSTALL ${libraries} DESTINATION ${CMAKE_INSTALL_PREFIX}/${PATH_SCRIPTS})
