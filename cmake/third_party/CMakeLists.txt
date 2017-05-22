
project(third_party_libraries)

cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/install CACHE PATH "Installation location")
set(COMMON_OPTIONS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION})
set(OPTIONS_TO_INHERITE
    CMAKE_AR
    CMAKE_BUILD_TYPE
    CMAKE_CXX_COMPILER
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_COMPILER
    CMAKE_C_FLAGS_RELEASE
    CMAKE_EXE_LINKER_FLAGS_RELEASE
    CMAKE_LINKER
    CMAKE_MAKE_PROGRAM
    CMAKE_MODULE_LINKER_FLAGS_RELEASE
    CMAKE_NM
    CMAKE_OBJCOPY
    CMAKE_OBJDUMP
    CMAKE_RANLIB
    CMAKE_RC_COMPILER
    CMAKE_SHARED_LINKER_FLAGS_RELEASE
    CMAKE_STRIP
)

foreach(VAR IN LISTS OPTIONS_TO_INHERITE)
    list(APPEND COMMON_OPTIONS -D${VAR}=${${VAR}})
endforeach()

option(BUILD_EASYEXIF    "Build easyexif.")
option(BUILD_JSONCPP     "Build jsoncpp.")
option(BUILD_OPENLIBRARY "Build openlibrary.")
option(BUILD_EXIV2       "Build Exiv2.")

find_program(GIT_EXECUTABLE git)
find_program(PATCH_EXECUTABLE patch)

# EASYEXIF
if(BUILD_EASYEXIF)
    ExternalProject_Add(easyexif
        GIT_REPOSITORY https://github.com/mayanklahiri/easyexif.git
        GIT_TAG v1.0
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/easyexif/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/easyexif && ${GIT_EXECUTABLE} add -A ${PROJECT_BINARY_DIR}/src/easyexif
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()


# JSONCPP
if(BUILD_JSONCPP)
    ExternalProject_Add(jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG 9234cbbc90d1f6c70dd5a90b4d533779e45c820c
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        )
endif()


# OPEN LIBRARY
if(BUILD_OPENLIBRARY)
    ExternalProject_Add(openlibrary
        GIT_REPOSITORY https://github.com/Kicer86/openlibrary.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()


# EXIV2
if(BUILD_EXIV2)

    cmake_minimum_required(VERSION 3.7)

    ExternalProject_Add(zlib
        URL http://zlib.net/zlib-1.2.11.tar.xz
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
    )

    ExternalProject_Add(expat
        GIT_REPOSITORY https://github.com/libexpat/libexpat
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS} -DBUILD_tests=FALSE -DCMAKE_DEBUG_POSTFIX=
        SOURCE_SUBDIR expat
    )

    ExternalProject_Add(exiv2
        DEPENDS zlib expat
        GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        PATCH_COMMAND ${PATCH_EXECUTABLE} -p0 < ${CMAKE_SOURCE_DIR}/exiv2/debug-build.patch
    )

endif()
