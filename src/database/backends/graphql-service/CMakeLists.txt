
include(FetchContent)

FetchContent_Declare(
  cppgraphqlgen
  GIT_REPOSITORY https://github.com/microsoft/cppgraphqlgen.git
  GIT_TAG        8c1623acc42392ef2a1bc0336482621386f98c77 # v4.5.0
  FIND_PACKAGE_ARGS NAMES cppgraphqlgen
)

FetchContent_MakeAvailable(cppgraphqlgen)

if(NOT TARGET taocpp::pegtl)
    find_package(pegtl     REQUIRED)            #required by cppgraphqlgen
endif()

find_program(python python REQUIRED)

add_custom_command(
    OUTPUT graphql_files.list
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory graphql
    COMMAND $<TARGET_FILE:cppgraphqlgen::schemagen>
    ARGS --schema ${CMAKE_CURRENT_SOURCE_DIR}/photos.graphql
         --prefix photo_broom
         --namespace database
         --source-dir graphql
         --header-dir graphql
         --no-introspection
         > graphql_files.list
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS photos.graphql
)

add_custom_command(
    OUTPUT graphql_files.cpp
    COMMAND ${python}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/collect_graphql_files.py
         ${CMAKE_CURRENT_BINARY_DIR}/graphql_files.list
         ${CMAKE_CURRENT_BINARY_DIR}/graphql_files.cpp
    DEPENDS graphql_files.list
)

add_library(
    graphql-service STATIC
    query_parser.cpp
    query_parser.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/graphql_files.cpp
)

target_include_directories(
    graphql-service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
    graphql-service
    PUBLIC
        core
        database

        cppgraphqlgen::graphqlservice
        cppgraphqlgen::graphqljson
)
