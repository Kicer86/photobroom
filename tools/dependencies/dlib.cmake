
find_package(PythonInterp REQUIRED)

ExternalProject_Add(dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG v19.13
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
                    ${PYTHON_EXECUTABLE} setup.py install --prefix=${CMAKE_INSTALL_PREFIX}
)
