
project(third_party_libraries)

cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/install)
set(COMMON_OPTIONS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION} 
                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                   -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM})
                   
option(BUILD_EASYEXIF    "Build easyexif.")
option(BUILD_HASHLIBRARY "Build hashlibrary.")
option(BUILD_JSONCPP     "Build jsoncpp.")
option(BUILD_OPENLIBRARY "Build openlibrary.")
 
find_program(GIT_EXECUTABLE git)
       
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
        CMAKE_ARGS ${COMMON_OPTIONS}
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
